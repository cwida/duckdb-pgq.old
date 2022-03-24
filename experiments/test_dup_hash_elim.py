import duckdb

conn = duckdb.connect()

conn.execute("PRAGMA enable_profiling")

conn.execute("CREATE TABLE Customer(cid bigint not null, name string);")

conn.execute("CREATE TABLE Transfers(tid bigint, from_id bigint, to_id bigint, amount bigint);")

conn.execute("INSERT INTO Customer VALUES (173, 'A'), (478, 'B'), (297, 'C'), (869, 'D');")

conn.execute("SELECT v.vcount FROM (SELECT count(c.cid) as vcount FROM Customer c) v;")

conn.execute("SELECT sum(CREATE_CSR_VERTEX(0, (SELECT count(c.cid) as vcount FROM Customer c), sub.dense_id , sub.cnt)) AS numEdges "
             "FROM ( SELECT c.rowid as dense_id, count(t.from_id) as cnt FROM Customer c "
             "LEFT JOIN  Transfers t ON t.from_id = c.cid GROUP BY c.rowid) sub")
