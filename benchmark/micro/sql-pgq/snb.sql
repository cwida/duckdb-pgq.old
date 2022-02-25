CREATE TABLE Person(PersonId bigint NOT NULL);
COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf0.1-projected-fk/Person.csv'       ( HEADER, FORMAT csv);

-- COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf0.1-projected-fk/Person.csv'       ( HEADER, FORMAT csv);
CREATE TABLE Person_knows_Person      (Person1Id bigint NOT NULL, Person2Id    bigint NOT NULL);
COPY Person_knows_Person (Person1id, Person2id) FROM 'data/ldbc-snb/social-network-sf0.1-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);
COPY Person_knows_Person (Person2id, Person1id) FROM 'data/ldbc-snb/social-network-sf0.1-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

CREATE TABLE src_dest(id int, is_variant boolean, v_size bigint, src bigint, dst bigint);


-- INSERT INTO src_dest (id, is_variant, v_size, src, dst) 
-- WITH cte as (
-- 	SELECT count(p.personid) as vcount FROM Person p
-- 	)
-- SELECT 0, false, cte.vcount, p1.rowid, p2.rowid FROM cte, Person p1, Person p2 ORDER BY md5(pl.rowid + pr.rowid) LIMIT 1000000;


CREATE TABLE Person_Subset_l(rid bigint);
INSERT INTO Person_Subset_l( SELECT person.rowid from Person ORDER BY md5(person.rowid) LIMIT 1000);


CREATE TABLE Person_Subset_r(rid bigint);
INSERT INTO Person_Subset_r( SELECT person.rowid from Person ORDER BY md5(person.rowid) LIMIT 1000);
-- EXPORT DATABASE '.' (FORMAT CSV, DELIMITER '|');

CREATE TABLE Person_ids(lid bigint, rid bigint);
INSERT INTO Person_ids( SELECT pl.rid, pr.rid from Person_Subset_l pl, Person_Subset_r pr ORDER BY md5(pl.rowid + pr.rowid) );
INSERT INTO Person_ids VALUES (0 , 0);

INSERT INTO src_dest (id, is_variant, v_size, src, dst) 
WITH cte as (
	SELECT count(p.personid) as vcount FROM Person p
	)
SELECT 0, false, cte.vcount, p.lid, p.rid FROM cte, Person_ids p;

CREATE TABLE t (id bigint);
INSERT INTO t VALUES(1);

-- INSERT INTO src_dest (id, is_variant, v_size, src, dst) 
-- WITH cte as (
-- 	SELECT count(p.personid) as vcount FROM Person p
-- 	)
-- SELECT 0, false, cte.vcount, p1.rowid, p2.rowid FROM cte, Person p1, Person p2  LIMIT 1000000;




SELECT min(CREATE_CSR_EDGE(0, (SELECT count(p.personid) as vcount FROM Person p), 
CAST ((SELECT sum(CREATE_CSR_VERTEX(0, (SELECT count(p.personid) as vcount FROM Person p), 
sub.dense_id , sub.cnt )) AS numEdges
FROM (
    SELECT p.rowid as dense_id, count(pkp.person1id) as cnt
    FROM Person p  
    LEFT JOIN  Person_knows_Person pkp ON pkp.person1id = p.personid
    GROUP BY p.rowid
) sub) AS BIGINT), 
src.rowid, dst.rowid ))
FROM 
  Person_knows_Person pkp 
  JOIN Person src ON pkp.person1id = src.personid
  JOIN Person dst ON pkp.person2id = dst.personid;


-- WITH cte as (
-- 	SELECT min(id) AS id FROM t
-- 	),
-- 	cte2 AS (
-- 	SELECT count(p.personid) AS vcount FROM Person p
-- 	)
-- SELECT min(src) from cte, cte2, src_dest where reach(src_dest.id, is_variant, cte2.vcount, src, dst, 'benchmark/micro/sql-pgq/debug/thread/0.1/msbfs/sf0.1_thread4_2.out') = cte.id;


-- PRAGMA threads=4;
-- PRAGMA enable_profiling=json;
-- PRAGMA profiling_output='/home/tavneet/vu/year_2/thesis/duckdb/profiling/b.json';