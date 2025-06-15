<?php
// Session başlatma veya gerekli dosyaları dahil etme işlemleri

include_once('../includes/config.php');

// ThingSpeak API Key ve Channel ID
$api_key = 'EW64WSSTLEVFN0ME';
$channel_id = '2571499';
$results = 200; // Kaç sonuç çekileceği

// Veritabanından alan verilerini çekme
$sql = "SELECT * FROM targo";
$result = $conn->query($sql);

$tarla_verileri = [];
if ($result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        $tarla_verileri[] = $row;
    }
}

// ThingSpeak verilerini alma fonksiyonu
function fetch_thingspeak_data($channel_id, $api_key, $results) {
    $url = "https://api.thingspeak.com/channels/$channel_id/feeds.json?api_key=$api_key&results=$results";
    $response = file_get_contents($url);
    return json_decode($response, true);
}

$thingspeak_data = fetch_thingspeak_data($channel_id, $api_key, $results);

$compare_results = [];
if ($thingspeak_data && isset($thingspeak_data['feeds'])) {
    foreach ($thingspeak_data['feeds'] as $feed) {
        $comment = '';

        foreach ($tarla_verileri as $tarla) {
            $comments = [];
            if (isset($feed['field1']) && $feed['field1'] > $tarla['dis_sicaklik']) {
                $comments[] = 'Dış sıcaklık değeri çok fazla!';
            }
            if (isset($feed['field2']) && $feed['field2'] > $tarla['nem']) {
                $comments[] = 'Nem değeri çok fazla!';
            }
            if (isset($feed['field3']) && $feed['field3'] > $tarla['ic_sicaklik']) {
                $comments[] = 'İç sıcaklık değeri çok fazla!';
            }
            if (isset($feed['field4']) && $feed['field4'] > $tarla['azot']) {
                $comments[] = 'Azot değeri çok fazla!';
            }
            if (isset($feed['field5']) && $feed['field5'] > $tarla['fosfor']) {
                $comments[] = 'Fosfor değeri çok fazla!';
            }
            if (!empty($comments)) {
                $comment = implode(' ', $comments);
            }
        }

        $compare_results[] = [
            'entry_id' => $feed['entry_id'],
            'field1' => $feed['field1'],
            'field2' => $feed['field2'],
            'field3' => $feed['field3'],
            'field4' => $feed['field4'],
            'field5' => $feed['field5'],
            'created_at' => date('Y-m-d H:i:s', strtotime($feed['created_at'])),
            'comment' => $comment
        ];
    }
}

header('Content-Type: application/json');
echo json_encode($compare_results);
?>
