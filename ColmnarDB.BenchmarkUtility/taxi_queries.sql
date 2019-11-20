SELECT cab_type, COUNT(*) FROM trips GROUP BY cab_type ORDER BY cab_type;
40
SELECT passenger_count, AVG(total_amount) FROM trips GROUP BY passenger_count ORDER BY passenger_count;
85
SELECT passenger_count, YEAR(pickup_datetime) AS pickup_year, COUNT(*) FROM trips GROUP BY passenger_count, pickup_year ORDER BY pickup_year, passenger_count;
930
SELECT passenger_count, YEAR(pickup_datetime) AS pickup_year, CAST(trip_distance AS INT) AS distance, COUNT(*) AS the_count FROM trips GROUP BY passenger_count, pickup_year, distance ORDER BY pickup_year, the_count DESC;
1120