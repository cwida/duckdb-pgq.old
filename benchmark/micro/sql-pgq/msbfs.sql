CREATE TABLE Customer(cid bigint not null);
CREATE TABLE Transfers(from_id bigint, to_id bigint, amount bigint);
CREATE TABLE src_dest(id int, v_size bigint, src bigint, dst bigint);
CREATE TABLE src(src_id bigint);

-- has to be same as v_size being used in src_dest
INSERT INTO Customer SELECT range FROM range(0, 10000);

INSERT INTO Transfers (from_id, to_id, amount) SELECT c1.cid  , c2.cid, 10 FROM Customer c1, Customer c2 LIMIT 400000;

INSERT INTO src_dest (id, v_size, src, dst) SELECT 0, 1000, c1.cid, c2.cid FROM Customer c1, Customer c2 LIMIT 400000;


SELECT CREATE_CSR_EDGE(0, (SELECT count(c.cid) as vcount FROM Customer c), 
CAST ((SELECT sum(CREATE_CSR_VERTEX(0, (SELECT count(c.cid) as vcount FROM Customer c), 
sub.dense_id , sub.cnt )) AS numEdges
FROM (
    SELECT c.rowid as dense_id, count(t.from_id) as cnt
    FROM Customer c  
    LEFT JOIN  Transfers t ON t.from_id = c.cid
    GROUP BY c.rowid
) sub) AS BIGINT), 
src.rowid, dst.rowid )
FROM 
  Transfers t 
  JOIN Customer src ON t.from_id = src.cid
  JOIN Customer dst ON t.to_id = dst.cid








