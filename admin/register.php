<?php
session_start();

error_reporting(E_ALL);
ini_set('display_errors', 1);

include_once('../includes/config.php');

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Formdan gelen verileri al
    $user = mysqli_real_escape_string($conn, $_POST['user']);
    $password = mysqli_real_escape_string($conn, $_POST['password']);
    $mail = mysqli_real_escape_string($conn, $_POST['mail']);

    $hashed_password = password_hash($password, PASSWORD_DEFAULT);

    $sql_insert_user = "INSERT INTO user (user, password, mail) VALUES ('$user', '$hashed_password', '$mail')";

    if ($conn->query($sql_insert_user) === TRUE) {
        header('Location: ../HTML/profile.html'); 
        exit;
    } else {
        echo "Kayıt sırasında bir hata oluştu: " . $conn->error;
    }
}

// Veritabanı bağlantısını kapatma
$conn->close();
?>
