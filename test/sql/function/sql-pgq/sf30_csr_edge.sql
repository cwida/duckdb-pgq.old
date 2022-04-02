PRAGMA enable_profiling;

CREATE TABLE person (id bigint);

CREATE TABLE person_knows_person (source bigint, target bigint);

copy person_knows_person from '/home/daniel/Documents/Programming/duckdb-pgq/data/csv/sf30/Person_knows_Person.csv' (HEADER, DELIMITER '|');

insert into knows select target, source from knows;

copy person from '/home/daniel/Documents/Programming/duckdb-pgq/data/csv/sf30/Person.csv' (HEADER);

SELECT CREATE_CSR_VERTEX(
               0,
               v.vcount,
               sub.dense_id,
               sub.cnt
           ) AS numEdges
FROM (
         SELECT p.rowid as dense_id, count(k.source) as cnt
         FROM person p
                  LEFT JOIN  knows k ON k.source = p.id
         GROUP BY p.rowid
     ) sub,  (SELECT count(p.id) as vcount FROM person p) v;

SELECT min(CREATE_CSR_EDGE(0, (SELECT count(p.id) as vcount FROM person p),
                           CAST ((SELECT sum(CREATE_CSR_VERTEX(0, (SELECT count(p.id) as vcount FROM person p),
                                                               sub.dense_id , sub.cnt )) AS numEdges
                                  FROM (
                                           SELECT p.rowid as dense_id, count(k.source) as cnt
                                           FROM person p
                                                    LEFT JOIN  knows k ON k.source = p.id
                                                    WHERE k.source IS NULL
                                           GROUP BY p.rowid
                                       ) sub) AS BIGINT),
                           src.rowid, dst.rowid ))
FROM
    knows k
        JOIN person src ON k.source = src.id
        JOIN person dst ON k.target = dst.id;

.quit
-- SELECT p.rowid as dense_id, count(k.source) as cnt
--     FROM person p
--     LEFT JOIN  knows k ON k.source = p.id
--     WHERE k.source IS NULL
--     GROUP BY p.rowid
--
-- src.rowid, dst.rowid ))
-- FROM
--     knows k
--         JOIN person src ON k.source = src.id
--         JOIN person dst ON k.target = dst.id;