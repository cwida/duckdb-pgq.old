CREATE TABLE Person(PersonId bigint NOT NULL);
COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf0.3-projected-fk/Person.csv'       ( HEADER, FORMAT csv);

-- COPY Person(PersonId) FROM  'data/ldbc-snb/social-network-sf0.3-projected-fk/Person.csv'       ( HEADER, FORMAT csv);
CREATE TABLE Person_knows_Person      (Person1Id bigint NOT NULL, Person2Id    bigint NOT NULL);
COPY Person_knows_Person (Person1id, Person2id) FROM 'data/ldbc-snb/social-network-sf0.3-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);
COPY Person_knows_Person (Person2id, Person1id) FROM 'data/ldbc-snb/social-network-sf0.3-projected-fk/Person_knows_Person.csv'       (DELIMITER '|', HEADER, FORMAT csv);

CREATE TABLE src_dest(id int, v_size bigint, src bigint, dst bigint);

INSERT INTO src_dest (id, v_size, src, dst) 
	SELECT 0, (SELECT count(p.personid) as vcount FROM Person p), p1.rowid, p2.rowid FROM Person p1, Person p2 ORDER BY RANDOM() LIMIT 1000000;


SELECT CREATE_CSR_EDGE(0, (SELECT count(p.personid) as vcount FROM Person p), 
CAST ((SELECT sum(CREATE_CSR_VERTEX(0, (SELECT count(p.personid) as vcount FROM Person p), 
sub.dense_id , sub.cnt )) AS numEdges
FROM (
    SELECT p.rowid as dense_id, count(pkp.person1id) as cnt
    FROM Person p  
    LEFT JOIN  Person_knows_Person pkp ON pkp.person1id = p.personid
    GROUP BY p.rowid
) sub) AS BIGINT), 
src.rowid, dst.rowid )
FROM 
  Person_knows_Person pkp 
  JOIN Person src ON pkp.person1id = src.personid
  JOIN Person dst ON pkp.person2id = dst.personid

