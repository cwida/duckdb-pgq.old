-- CREATE TABLE Customer(cid bigint not null);
-- CREATE TABLE Transfers(from_id bigint, to_id bigint, amount bigint);
CREATE TABLE Person(PersonId bigint NOT NULL);
COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf0.1-projected-fk/Person.csv'       ( HEADER, FORMAT csv);

-- COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf0.1-projected-fk/Person.csv'       ( HEADER, FORMAT csv);
CREATE TABLE Person_knows_Person      (Person1Id bigint NOT NULL, Person2Id    bigint NOT NULL);
COPY Person_knows_Person (Person1id, Person2id) FROM 'data/ldbc-snb/social-network-sf0.1-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);
COPY Person_knows_Person (Person2id, Person1id) FROM 'data/ldbc-snb/social-network-sf0.1-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

-- INSERT INTO Customer SELECT range FROM range(0, 10000) ORDER BY RANDOM();

-- CREATE TEMPORARY TABLE clist(cid bigint);
-- INSERT INTO clist SELECT DISTINCT(cid) from Customer;


-- INSERT INTO Transfers (from_id, to_id, amount) SELECT c1.cid  , c2.cid, 10 FROM Customer c1, Customer c2 ;
