import React, { useState, useEffect } from 'react';
import './App.css'
import Header from './Header.jsx';
import Footer from './footer.jsx';
import { ToastContainer, toast } from 'react-toastify';
import LoginPage from './LoginPage.jsx';
import Dashboard from './Dashboard.jsx';


function App() {
    const [username, setUsername] = useState('');
    const [isLoggedin,setLoggedin] = useState(false)
    const [message, setMessage] = useState('')

    useEffect(() => {
      fetch('http://localhost:18080/api/ping')
      .then(res => res.json())
      .then(data => setMessage(data.message))
      .catch(err => setMessage("Error: " + err.message));
    }, []);

  return(
    <>
    <title>Banking System</title>
    <Header/>

    <div>
      <ToastContainer />
    </div>

    {isLoggedin 
      ? <Dashboard setLoggedin={setLoggedin} username = {username}/> 
      : <LoginPage onLoginSuccess={() => setLoggedin(true)} setUser = {setUsername}/>
    }
    <Footer/>

    </>
  );
}

export default App
