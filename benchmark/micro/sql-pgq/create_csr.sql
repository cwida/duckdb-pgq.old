CREATE TABLE Customer(cid bigint not null);
CREATE TABLE Transfers(from_id bigint, to_id bigint, amount bigint);
INSERT INTO Customer SELECT range FROM range(0, 10000) ORDER BY RANDOM();

-- CREATE TEMPORARY TABLE clist(cid bigint);
-- INSERT INTO clist SELECT DISTINCT(cid) from Customer;


INSERT INTO Transfers (from_id, to_id, amount) SELECT c1.cid  , c2.cid, 10 FROM Customer c1, Customer c2 ;
