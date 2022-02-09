CREATE TABLE Person(PersonId bigint NOT NULL);

COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf0.1-projected-fk/Person.csv'       ( HEADER, FORMAT csv);

CREATE TABLE Person_knows_Person      (Person1Id bigint NOT NULL, Person2Id    bigint NOT NULL);

COPY Person_knows_Person (Person1id, Person2id) FROM 'data/ldbc-snb/social-network-sf0.1-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

COPY Person_knows_Person (Person2id, Person1id) FROM 'data/ldbc-snb/social-network-sf0.1-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

CREATE TABLE Person_Subset_l(rid bigint);
INSERT INTO Person_Subset_l( SELECT person.rowid from Person ORDER BY RANDOM() LIMIT 1000);

CREATE TABLE Person_Subset_r(rid bigint);
INSERT INTO Person_Subset_r( SELECT person.rowid from Person ORDER BY RANDOM() LIMIT 1000);

