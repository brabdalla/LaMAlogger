<?php

if (isset($_GET['temp'])) {

    $latitude = NULL;
    $longitude = NULL;
    $sensor = NULL;
// receives Arduino Data
    $temperatura = $_GET['temp'];
    $umidade = $_GET['umid'];
    $latitude = $_GET['lat'];
    $longitude = $_GET['lon'];
    $altitude = $_GET['alt'];
    $sensor = $_GET['id'];
    
// EDIT: Your mysql database account information
    $username = "root";  // user
    $password = "lama";  // password
    $database = "lamalogdb"; // db
    $tablename = "dados"; // table name
    $localhost = "localhost"; // ip

// Check Connection to Database
    if (mysql_connect($localhost, $username, $password)) {
        @mysql_select_db($database) or die("Unable to select database");
        //echo '\ninserido no bd';
        // Next two lines will write into your table 'test_table_name_here' with 'yourdata' value from the arduino and will timestamp that data using 'now()'
        $query = "INSERT INTO $tablename (sensor_id,temperatura,umidade,latitude,longitude,altitude) VALUES ($sensor,$temperatura,$umidade,$latitude,$longitude,$altitude)";

        $result = mysql_query($query);
        echo $query;
    } else {
        echo('Unable to connect to database.');
    }
}
?>