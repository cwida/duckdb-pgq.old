CREATE TABLE Person(PersonId bigint NOT NULL);

COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf0.3-projected-fk/Person.csv'       ( HEADER, FORMAT csv);

CREATE TABLE Person_knows_Person      (Person1Id bigint NOT NULL, Person2Id    bigint NOT NULL);

COPY Person_knows_Person (Person1id, Person2id) FROM 'data/ldbc-snb/social-network-sf0.3-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

COPY Person_knows_Person (Person2id, Person1id) FROM 'data/ldbc-snb/social-network-sf0.3-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

-- SELECT setseed(0.1);

CREATE TABLE Person_Subset_l(rid bigint);
-- INSERT INTO Person_Subset_l VALUES (0);
INSERT INTO Person_Subset_l( SELECT person.rowid from Person ORDER BY md5(person.rowid) LIMIT 999);

-- INSERT INTO Person_Subset_l( SELECT person.rowid from Person LIMIT 1000);


CREATE TABLE Person_Subset_r(rid bigint);
-- INSERT INTO Person_Subset_r( SELECT person.rowid from Person LIMIT 1000);
INSERT INTO Person_Subset_r( SELECT person.rowid from Person ORDER BY md5(person.rowid) LIMIT 1000);
-- EXPORT DATABASE '.' (FORMAT CSV, DELIMITER '|');

CREATE TABLE Person_ids(lid bigint, rid bigint);
INSERT INTO Person_ids( SELECT pl.rid, pr.rid from Person_Subset_l pl, Person_Subset_r pr ORDER BY md5(pl.rowid + pr.rowid) );
INSERT INTO Person_ids VALUES (0 , 0);

-- INSERT INTO Person_ids2( SELECT pl2.rid, pr2.rid from pl2, pr2 ORDER BY md5(pl2.rowid + pr2.rowid) );
-- SELECT src.rid, dst.rid
-- FROM Person_Subset_l src, Person_Subset_r dst 
-- WHERE  reach(0, false, 3900, src.rid, dst.rid, 'sf0.3_lane256_thread2_variant10.out') = 0);

-- INSERT INTO reachable_all(
-- SELECT src.rowid, dst.rowid
-- FROM Person src, Person dst 
-- WHERE  reach(0, false, 3900, src.rowid, dst.rowid, 'sf0.3_lane256_thread2_variant10.out') = 1);
-- LIMIT 10;

-- CREATE TABLE Person(PersonId bigint NOT NULL);

-- COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf0.3-projected-fk/Person.csv'       ( HEADER, FORMAT csv);
-- SELECT setseed(0.1);

CREATE TABLE Pl2(rid bigint);
INSERT INTO Pl2( SELECT person.rowid from Person ORDER BY md5(person.rowid) LIMIT 999);

-- CREATE TABLE Pr(rid bigint);
-- INSERT INTO Pr( SELECT person.rowid from Person ORDER BY RANDOM() LIMIT 999);

-- SELECT count(*) FROM pl JOIN  pr ON pl.rid = pr.rid;

-- INSERT INTO Pr 
-- SELECT i
-- FROM
--   (SELECT unnest(generate_series(1, 999)) AS i)
-- ORDER BY md5(i)