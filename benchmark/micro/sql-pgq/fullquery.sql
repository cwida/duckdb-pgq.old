CREATE TABLE Person(PersonId bigint NOT NULL);

COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf0.3-projected-fk/Person.csv'       ( HEADER, FORMAT csv);

CREATE TABLE Person_knows_Person      (Person1Id bigint NOT NULL, Person2Id    bigint NOT NULL);

COPY Person_knows_Person (Person1id, Person2id) FROM 'data/ldbc-snb/social-network-sf0.3-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

COPY Person_knows_Person (Person2id, Person1id) FROM 'data/ldbc-snb/social-network-sf0.3-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

-- SELECT setseed(0.1);

CREATE TABLE Person_Subset_l(rid bigint);
INSERT INTO Person_Subset_l( SELECT person.rowid from Person ORDER BY md5(person.rowid) LIMIT 1000);


CREATE TABLE Person_Subset_r(rid bigint);
INSERT INTO Person_Subset_r( SELECT person.rowid from Person ORDER BY md5(person.rowid) LIMIT 1000);
-- EXPORT DATABASE '.' (FORMAT CSV, DELIMITER '|');

CREATE TABLE Person_ids(lid bigint, rid bigint);
INSERT INTO Person_ids( SELECT pl.rid, pr.rid from Person_Subset_l pl, Person_Subset_r pr ORDER BY md5(pl.rowid + pr.rowid) );
INSERT INTO Person_ids VALUES (0 , 0);

-- PRAGMA enable_profiling;
-- PRAGMA profiling_output='/home/tavneet/vu/year_2/thesis/duckdb/profiling/8.out';
-- INSERT INTO Person_ids2( SELECT pl2.rid, pr2.rid from pl2, pr2 ORDER BY md5(pl2.rowid + pr2.rowid) );
-- SELECT src.rid, dst.rid
-- FROM Person_Subset_l src, Person_Subset_r dst 
-- WHERE  reach(0, false, 3900, src.rid, dst.rid, 'sf0.3_lane256_thread2_variant10.out') = 0);
CREATE TABLE verify_pairs_full (p1id bigint, p2id bigint);

-- INSERT INTO reachable_all(
-- SELECT src.personid, dst.personid
-- FROM Person src, Person dst 
-- WHERE  reach(0, false, 1700, src.rowid, dst.rowid, 'sf0.1_lane64_thread1_variant10.out') = 1);

-- CREATE TABLE reachable_all (p1id bigint, p2id bigint);

-- INSERT INTO reachable_all(
-- WITH cte1 AS (
-- SELECT min(CREATE_CSR_EDGE(0, (SELECT count(p.personid) as vcount FROM Person p), 
-- CAST ((SELECT sum(CREATE_CSR_VERTEX(0, (SELECT count(p.personid) as vcount FROM Person p), 
-- sub.dense_id , sub.cnt )) AS numEdges
-- FROM (
--     SELECT p.rowid as dense_id, count(pkp.person1id) as cnt
--     FROM Person p  
--     LEFT JOIN  Person_knows_Person pkp ON pkp.person1id = p.personid
--     GROUP BY p.rowid
-- ) sub) AS BIGINT) , src.rowid, dst.rowid)) as temp, (SELECT count(p.personid) FROM Person p) as vcount 
-- FROM 
--   Person_knows_Person pkp 
--   JOIN Person src ON pkp.person1id = src.personid
--   JOIN Person dst ON pkp.person2id = dst.personid
-- )
-- SELECT src.personid, dst.personid
-- FROM cte1, Person src, Person dst
-- WHERE  reach(0, false, cte1.vcount, src.rowid, dst.rowid, 'temp.out') = cte1.temp );

-- LIMIT 10;

-- CREATE TABLE Person(PersonId bigint NOT NULL);

-- COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf0.3-projected-fk/Person.csv'       ( HEADER, FORMAT csv);
-- SELECT setseed(0.1);

-- CREATE TABLE Pl2(rid bigint);
-- INSERT INTO Pl2( SELECT person.rowid from Person ORDER BY md5(person.rowid) LIMIT 999);

-- CREATE TABLE Pr(rid bigint);
-- INSERT INTO Pr( SELECT person.rowid from Person ORDER BY RANDOM() LIMIT 999);

-- SELECT count(*) FROM pl JOIN  pr ON pl.rid = pr.rid;

-- INSERT INTO Pr 
-- SELECT i
-- FROM
--   (SELECT unnest(generate_series(1, 999)) AS i)
-- ORDER BY md5(i)