import React, { useState, useEffect } from 'react';
import './App.css'
import Button from './button.jsx';
import InputCredential from './inputEmail.jsx';
import notify from './ToastNotify.jsx';


function LoginPage({onLoginSuccess, setUser}){
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');

  return(
  <>
  <div className = 'signInFields'>
    <div className = 'inputUsername'>
      <InputCredential
        label='Username'
        value={username}
        onChange={(e) => setUsername(e.target.value)}
        className='usernameField'
      />
      <span className="tooltip">Enter your username</span>
    </div>
    <div className = 'inputPassword'>
      <InputCredential
        label='Password'
        value={password}
        onChange={(e) => setPassword(e.target.value)}
        className='passwordField'
      />
      <span className="tooltip1">
        Password must contain:
        <br/>&bull; 8 Characters
        <br/>&bull; Uppercase letter
        <br/>&bull; Lowercase letter
        <br/>&bull; Number
        <br/>&bull; Special Character
        </span>
    </div>
  </div>

  <div className = 'signInMenu'>
    <Button
      label="Sign In"
      onClick={() => {
        fetch("http://localhost:18080/api/signin", {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({
            username: username,
            password: password,
          })
        })
        .then(res => res.json())
        .then(data => {
          notify(data.message);
          {if (data.message == 'Successfully Signed in!'){
            console.log(`${username} has signed in!`)
            setUser(username)
            onLoginSuccess()
          }}
        })
        .catch(err => notify("Sign in failed: " + err.message));
      }}
      className="signInButton"
    />

    <Button
      label="Create Account"
      onClick={() => {
        fetch("http://localhost:18080/api/createaccount",{
          method: 'POST',
          headers: {'Content-type': 'application/json'},
          body: JSON.stringify({
            username: username,
            password: password
          })
        })
        .then(res => res.json())
        .then(data => {
          notify(data.message);
        })
        .catch(err => notify("Create account failed: " + err.message));
      }}
      className="createAccountButton"
    />
  </div>
  </>)
}

export default LoginPage