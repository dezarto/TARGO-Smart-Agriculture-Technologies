<?php

// Your ThingSpeak Read API Key and Channel ID
$api_key = 'EW64WSSTLEVFN0ME';
$channel_id = '2571499';
$results = 2; // Number of results to fetch

// Database query to fetch field data
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ThingSpeak Data Display</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            line-height: 1.6;
            background-color: #f5f5f5;
            margin: 20px;
            padding: 20px;
        }
        h1, h2, p {
            color: #333;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
        }
        th, td {
            border: 1px solid #ccc;
            padding: 10px;
            text-align: left;
        }
        th {
            background-color: #f2f2f2;
        }
    </style>
</head>
<body>
    <h1>ThingSpeak Data</h1>
    <table border="1">
        <thead>
            <tr>
                <th>Entry ID</th>
                <th>Sıcaklık</th>
                <th>Nem</th>
                <th>Gaz</th>
                <th>Water</th>
                <th>soiltemp</th>
                <th>Time</th>
            </tr>
        </thead>
        <tbody id="data-table-body">
            <tr><td colspan="7">Loading data...</td></tr>
        </tbody>
    </table>

    <script>
        function fetchData() {
            // URL for reading ThingSpeak data from all fields
            const url = "https://api.thingspeak.com/channels/<?php echo $channel_id; ?>/feeds.json?api_key=<?php echo $api_key; ?>&results=<?php echo $results; ?>";
            const chart = "https://api.thingspeak.com/channels/<?php echo $channel_id; ?>/feeds.json?api_key=<?php echo $api_key; ?>1";
            fetch(url)
                .then(response => response.json())
                .then(data => {
                    const feeds = data.feeds;
                    let tableBody = document.getElementById('data-table-body');
                    tableBody.innerHTML = ''; // Clear existing data

                    if (feeds && feeds.length > 0) {
                        feeds.forEach(feed => {
                            let row = `<tr>
                                <td>${feed.entry_id}</td>
                                <td>${feed.field1}</td>
                                <td>${feed.field2}</td>
                                <td>${feed.field3}</td>
                                <td>${feed.field4}</td>
                                <td>${feed.field5}</td>                              
                                <td>${new Date(feed.created_at).toLocaleString()}</td>
                            </tr>`;
                            tableBody.innerHTML += row;
                        });
                    } else {
                        tableBody.innerHTML = '<tr><td colspan="7">No data available</td></tr>';
                    }
                })
                .catch(error => console.error('Error fetching data:', error));
        }

        // Fetch data initially and then every 15 seconds
        document.addEventListener('DOMContentLoaded', (event) => {
            fetchData();
            setInterval(fetchData, 15000);
        });
    </script>
</body>
</html>
