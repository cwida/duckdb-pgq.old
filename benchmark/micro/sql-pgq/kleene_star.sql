
CREATE TABLE Person(PersonId bigint NOT NULL);

COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf3-projected-fk/Person.csv'       ( HEADER, FORMAT csv);

CREATE TABLE Person_knows_Person      (Person1Id bigint NOT NULL, Person2Id    bigint NOT NULL);

COPY Person_knows_Person (Person1id, Person2id) FROM 'data/ldbc-snb/social-network-sf3-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

COPY Person_knows_Person (Person2id, Person1id) FROM 'data/ldbc-snb/social-network-sf3-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

CREATE PROPERTY GRAPH aml1 
VERTEX TABLES ( person KEY ( personid ) LABEL person )
EDGE TABLES ( Person_knows_Person KEY ( Person1Id ) SOURCE KEY ( Person1Id ) REFERENCES Person DESTINATION KEY ( Person2Id ) REFERENCES Person LABEL Person_knows_Person )
