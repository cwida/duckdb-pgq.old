import duckdb

conn = duckdb.connect()

conn.execute("CREATE TABLE person (id bigint)")

conn.execute("CREATE TABLE knows (source bigint, target bigint)")

conn.execute(
    "copy knows from '/home/daniel/Documents/Programming/duckdb-pgq/data/csv/sf0.1/Person_knows_Person.csv' (HEADER, DELIMITER '|');")

conn.execute("insert into knows select target, source from knows")

conn.execute("copy person from '/home/daniel/Documents/Programming/duckdb-pgq/data/csv/sf0.1/Person.csv' (HEADER);")

conn.execute("create table hops (source bigint, target bigint, hops int);")

conn.execute(
    "copy hops from '/home/daniel/Documents/Programming/duckdb-pgq/data/csv/tmp/outputsf0.1.csv' (DELIMITER ',', HEADER);")

conn.execute(
    "SELECT CREATE_CSR_VERTEX(0,v.vcount,sub.dense_id,sub.cnt) AS numEdges FROM (     "
    "SELECT p.rowid as dense_id, count(k.source) as cnt "
    "FROM person p "
    "LEFT JOIN  knows k ON k.source = p.id "
    "GROUP BY p.rowid ) sub,  "
    "(SELECT count(p.id) as vcount FROM person p) v;")

conn.execute(
    "SELECT min(CREATE_CSR_EDGE(0, (SELECT count(p.id) as vcount FROM person p), "
    "CAST ((SELECT sum(CREATE_CSR_VERTEX(0, (SELECT count(p.id) as vcount FROM person p), sub.dense_id , sub.cnt )) "
    "AS numEdges FROM (     SELECT p.rowid as dense_id, count(k.source) as cnt FROM person p "
    "LEFT JOIN  knows k ON k.source = p.id GROUP BY p.rowid ) sub) AS BIGINT), src.rowid, dst.rowid )) "
    "FROM knows k JOIN person src ON k.source = src.id JOIN person dst ON k.target = dst.id;")

conn.execute("CREATE TABLE src_dest(id int default 0, v_size bigint, src bigint, dst bigint);")

conn.execute(
    "create table hops_rowid as select p.rowid as src_rowid, p2.rowid as dest_rowid, h.hops as hops from hops h JOIN person p on p.id = h.source JOIN person p2 on p2.id = h.target;")

conn.execute("insert into src_dest (src, dst) select src_rowid, dest_rowid from hops_rowid h;")

conn.execute("update src_dest s set v_size = (select count(*) from person p) where v_size is NULL;")

conn.execute(
    "SELECT p.id as source, p2.id as target, shortest_path(s.id, false, v_size, s.src, s.dst) as hops "
    "FROM src_dest s "
    "LEFT JOIN person p ON s.src = p.rowid "
    "LEFT JOIN person p2 ON s.dst = p2.rowid "
    "order by p.id, p2.id;")
