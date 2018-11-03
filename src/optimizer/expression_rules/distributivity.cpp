
#include <algorithm>
#include <vector>

#include "common/exception.hpp"
#include "common/internal_types.hpp"
#include "common/value_operations/value_operations.hpp"
#include "optimizer/rule.hpp"

#include "optimizer/expression_rules/distributivity.hpp"

using namespace duckdb;
using namespace std;

DistributivityRule::DistributivityRule() {
	root = make_unique<ExpressionNodeType>(ExpressionType::CONJUNCTION_OR);
	root->child_policy = ChildPolicy::SOME;
}

static void GatherAndExpressions(Expression *expression,
                                 vector<Expression *> &result) {
	if (expression->type == ExpressionType::CONJUNCTION_AND) {
		// gather expressions
		for (size_t i = 0; i < expression->children.size(); i++) {
			GatherAndExpressions(expression->children[i].get(), result);
		}
	} else {
		// just add the expression
		result.push_back(expression);
	}
}

static void GatherOrExpressions(Expression *expression,
                                vector<vector<Expression *>> &result) {
	assert(expression->type == ExpressionType::CONJUNCTION_OR);
	// traverse the children
	for (size_t i = 0; i < expression->children.size(); i++) {
		auto child = expression->children[i].get();
		if (child->type == ExpressionType::CONJUNCTION_OR) {
			GatherOrExpressions(child, result);
		} else {
			vector<Expression *> new_expressions;
			GatherAndExpressions(child, new_expressions);
			result.push_back(new_expressions);
		}
	}
}

static unique_ptr<Expression> Prune(unique_ptr<Expression> root) {
	if (root->type == ExpressionType::INVALID) {
		// prune this node
		return nullptr;
	}
	if (root->type == ExpressionType::CONJUNCTION_OR ||
	    root->type == ExpressionType::CONJUNCTION_AND) {
		// conjunction, prune recursively
		assert(root->children.size() == 2);
		root->children[0] = Prune(move(root->children[0]));
		root->children[1] = Prune(move(root->children[1]));
		if (root->children[0] && root->children[1]) {
			// don't prune
		} else if (root->children[0]) {
			// prune right
			return move(root->children[0]);
		} else if (root->children[1]) {
			// prune left
			return move(root->children[1]);
		} else {
			// prune entire node
			return nullptr;
		}
	}
	// no conjunction or invalid, just return the node again
	return root;
}

unique_ptr<Expression>
DistributivityRule::Apply(Rewriter &rewriter, Expression &root,
                          vector<AbstractOperator> &bindings,
                          bool &fixed_point) {
	auto initial_or = (ConjunctionExpression *)bindings[0].value.expr;
	// gather all the expressions inside AND expressions
	vector<vector<Expression *>> gathered_expressions;
	GatherOrExpressions(initial_or, gathered_expressions);

	// now we have a list of expressions we have gathered for this OR
	// if every list in this OR contains the same expression, we can extract
	// that expression
	// FIXME: this could be done more efficiently with a hashmap

	vector<int> matches;
	matches.resize(gathered_expressions.size());

	unique_ptr<Expression> new_root;
	for (size_t i = 0; i < gathered_expressions[0].size(); i++) {
		auto entry = gathered_expressions[0][i];
		matches[0] = i;
		bool occurs_in_all_expressions = true;
		for (size_t j = 1; j < gathered_expressions.size(); j++) {
			matches[j] = -1;
			for (size_t k = 0; k < gathered_expressions[j].size(); k++) {
				auto other_entry = gathered_expressions[j][k];
				if (entry->Equals(other_entry)) {
					// match found
					matches[j] = k;
					break;
				}
			}
			if (matches[j] < 0) {
				occurs_in_all_expressions = false;
				break;
			}
		}
		if (occurs_in_all_expressions) {
			assert(matches.size() >= 2);
			// this expression occurs in all expressions, we can push it up
			// before the main OR expression.

			// make a copy of the right child for usage in the root
			auto right_child = gathered_expressions[0][i]->Copy();

			// now we need to remove each matched entry from its parents.
			// for all nodes, set the ExpressionType to INVALID
			// we do the actual pruning later
			for (size_t m = 0; m < matches.size(); m++) {
				auto entry = gathered_expressions[m][matches[m]];
				entry->type = ExpressionType::INVALID;
			}

			unique_ptr<Expression> left_child;
			if (new_root) {
				// we already have a new root, set that as the left child
				left_child = move(new_root);
			} else {
				// no new root yet, create a new OR expression with the children
				// of the main root
				left_child = make_unique<ConjunctionExpression>(
				    ExpressionType::CONJUNCTION_OR,
				    move(initial_or->children[0]),
				    move(initial_or->children[1]));
			}
			new_root = make_unique<ConjunctionExpression>(
			    ExpressionType::CONJUNCTION_AND, move(left_child),
			    move(right_child));
		}
	}
	if (new_root) {
		// we made a new root
		// we need to prune invalid entries!
		return Prune(move(new_root));
	} else {
		return nullptr;
	}
}