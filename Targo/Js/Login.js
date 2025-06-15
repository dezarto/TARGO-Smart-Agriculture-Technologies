const signUpButton = document.getElementById('signUp');
const signInButton = document.getElementById('signIn');
const container = document.getElementById('container');

signUpButton.addEventListener('click', () => {
	container.classList.add("right-panel-active");
});

signInButton.addEventListener('click', () => {
	container.classList.remove("right-panel-active");
});

// Login.js
document.addEventListener('DOMContentLoaded', () => {
    const loginForm = document.getElementById('loginForm');
    
    loginForm.addEventListener('submit', (event) => {
      event.preventDefault(); // Formun normalde gönderilmesini engeller
  
      const email = document.getElementById('email').value;
      const password = document.getElementById('password').value;
  
      if (email === 'onur@gmail.com' && password === '1234') {
        window.location.href = '/HTML/HomePage.html'; // Ana sayfaya yönlendir
      } else {
        alert('Invalid email or password');
      }
    });
  });
  