CREATE TABLE Person(PersonId bigint NOT NULL);

COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf0.3-projected-fk/Person.csv'       ( HEADER, FORMAT csv);

CREATE TABLE Person_knows_Person      (Person1Id bigint NOT NULL, Person2Id    bigint NOT NULL);

COPY Person_knows_Person (Person1id, Person2id) FROM 'data/ldbc-snb/social-network-sf0.3-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

COPY Person_knows_Person (Person2id, Person1id) FROM 'data/ldbc-snb/social-network-sf0.3-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

SELECT setseed(0.1);

CREATE TABLE Person_Subset_l(rid bigint);
INSERT INTO Person_Subset_l VALUES (0);
INSERT INTO Person_Subset_l( SELECT person.rowid from Person ORDER BY RANDOM() LIMIT 999);

CREATE TABLE Person_Subset_r(rid bigint);
-- INSERT INTO Person_Subset_r( SELECT person.rowid from Person ORDLIMIT 1000);
INSERT INTO Person_Subset_r( SELECT person.rowid from Person ORDER BY RANDOM() LIMIT 1000);

