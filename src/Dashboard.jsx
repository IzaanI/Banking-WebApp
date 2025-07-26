import React, { useState, useEffect } from 'react';
import MoveMoneyField from "./MoveMoneyField"
import Button from "./button"
import notify from './ToastNotify.jsx';
import LoginPage from './LoginPage.jsx';
import MaxTransaction from './maxTransaction.jsx';

function Dashboard({setLoggedin, username}){
    const [moveAmount, setMoveAmount] = useState("") //withdraw and deposit
    const [moveAmountT, setMoveAmountT] = useState("") // T = transfer amount
    const [transferUser, setTransferUser] = useState("")

    const [balance, setBalance] = useState(0)
    const [age, setAge] = useState(0)
    const [income, setIncome] = useState(0)
    const [homeOwnership, setHomeOwnership] = useState(0)

    const [count, setCount] = useState(60)

    useEffect(() => {
        const interval = setInterval(() => {
            setCount((prev) => {
            if (prev <= 1) {
                clearInterval(interval);      // stop ticking
                setLoggedin(false);           // trigger logout
                notify("Logged out due to Inactivity")
                return 0;
            }
            return prev - 1;
            });
    }, 1000);

    return () => clearInterval(interval); // cleanup on unmount
    }, []);


    useEffect(() => {
        const handleMove = () => {
            setCount(60)
        };

        window.addEventListener("mousemove", handleMove);

        return () => {
            window.removeEventListener("mousemove", handleMove);
        };
    }, []);

    function roundTo(value, n) {
        return Number(Math.round(value + "e" + n) + "e-" + n);
    }



    useEffect(() => {
        // Fetch balance once on mount
        fetch("http://localhost:18080/api/balance", {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ username: username })
        })
        .then(res => res.json())
        .then(data => setBalance(data.balance))
        .catch(err => notify(err.message));
        }, []
    ); 


    useEffect(() => {
    }, [balance]);

    function validateInput(val, setter){
        // Allow empty string
        if (val === "") {
            setter("");
            return;
        }

        // Allow valid float-in-progress values
        const floatRegex = /^(\d+)?(\.)?(\d*)?$/;

        if (floatRegex.test(val)) {
            setter(val);
        }
    }

    return(
        <>
            {count < 30 &&
                <> 
                    <p>{count}</p>
                </> 
            }
            <Button 
                label = "Logout"
                onClick = {() => {
                    setLoggedin(false)
                    notify("Successfully Signed Out!")
                }}
                className="logout-button"
            />
            <div className ='cards-container'>
                <div className = 'card'>
                    <div className = 'account-info'>
                        <h2 className = 'account-balance-text'>Account Balance</h2>
                        <p className = 'balance'>${balance}</p>
                    </div>
                    <div className = 'account-info-right'>
                        <MoveMoneyField
                            label = 'Enter Amount'
                            className = 'move-money-field1' 
                            value = {moveAmount}
                            onChange={(e) => {
                                const val = e.target.value;
                                validateInput(val,setMoveAmount)
                            }}

                        />
                        <div className = 'account-info-buttons'>
                            <Button 
                                label ='Deposit' 
                                className = 'deposit-button'
                                onClick = {() =>{
                                    if(MaxTransaction(moveAmount) == false){
                                        notify("Max Transaction Quantity Exceeded. Please Enter a value less than $10,000")
                                    }else if(moveAmount !=""){
                                        fetch("http://localhost:18080/api/deposit", {
                                            method: 'POST',
                                            headers: {'Content-Type': 'application/json'},
                                            body: JSON.stringify({
                                                deposit:parseFloat(roundTo(moveAmount,2)),
                                                username:username
                                            })
                                        })
                                        .then(res => res.json())
                                        .then(data =>{
                                            notify(`Successfully Deposited: $${roundTo(data.message,2)}`)
                                            setBalance(data.balance)
                                        })
                                        .catch(err => notify("Deposit failed: " + err.message))
                                    }else {notify("Please Enter an Amount...")} 
                                }}
                            />
                            <Button 
                                label ='Withdraw' 
                                className = 'withdraw-button'
                                onClick={()=>{
                                    if(MaxTransaction(moveAmount) == false){
                                        notify("Max Transaction Quantity Exceeded. Please Enter a value less than $10,000")
                                    }else if(moveAmount !=""){
                                        fetch("http://localhost:18080/api/withdraw",{
                                            method:'POST',
                                            headers: {'Content-Type':'application/json'},
                                            body: JSON.stringify({
                                                withdraw:parseFloat(roundTo(moveAmount,2)),
                                                username:username
                                            })
                                        })
                                        .then(res=> res.json())
                                        .then(data =>{
                                            if (data.status === true){
                                            notify(`Succsessfully Withdrew $${roundTo(data.message,2)}`)
                                            setBalance(data.balance)
                                            }else{notify("Insuficient funds...")}
                                        })
                                        .catch(err => notify("Withdraw failed: " + err.message))
                                    }else {notify("Please Enter an Amount...")}
                                }}
                            />
                        </div>
                    </div>
                </div>
            </div>
            <div className= 'bottom-container'>
                <div className ='transfer-card'>
                    <h2 className ='quick-transfer-text'>Quick Transfer</h2>
                    <h2 
                        className ='amount-text'>Amount 
                        <MoveMoneyField 
                            label ='Enter Amount' 
                            className = 'transfer-amount-input'
                            value = {moveAmountT}
                            onChange={(e) => {
                                const val = e.target.value;
                                validateInput(val,setMoveAmountT)
                            }}
                        /> 
                    </h2>
                    <h2 className ='recipient-text'>Recipient 
                        <MoveMoneyField 
                            label ='Enter Username' 
                            className = 'recipient-amount-input'
                            onChange = {(e) =>{
                                setTransferUser(e.target.value)
                            }}
                        /> 
                    </h2>
                    <Button label='Send' 
                    className ='send-money-button'
                    onClick={()=>{
                        if(MaxTransaction(moveAmountT) == false){
                            notify("Max Transaction Quantity Exceeded. Please Enter a value less than $10,000")
                        }else if(moveAmountT !=""){
                            fetch("http://localhost:18080/api/transfer",{
                                method:'POST',
                                headers: {'Content-Type':'application/json'},
                                body: JSON.stringify({
                                    amount:parseFloat(roundTo(moveAmountT,2)),
                                    username:username,
                                    transferUser: transferUser
                                })
                            })
                            .then(res=> res.json())
                            .then(data =>{
                                if(data.message == "Recipient account not found..." || data.message == "Cannot Transfer to self..." || data.message =="Insufficient funds."){
                                    notify(data.message)
                                }else{
                                    notify(`Successfully transferred $${roundTo(moveAmountT,2)} from ${username} to ${transferUser}!`)
                                    setBalance(data.balance)
                                }
                            })
                            .catch(err => notify("Transfer failed: " + err.message))
                        }else{notify("Please Enter an Amount...")}
                    }}
                    />
                </div>
                <div className ='credit-card'>
                    <h2 className ='quick-transfer-text'>Credit Eligbility</h2>
                    <div className='credit-interactables'>
                        <div className='left-credit'>
                            <div className = 'age-elements'>
                            <label className ='age-text'htmlFor='age-range'>Age   </label>
                            <select
                                className ='age-dropdown'
                                id='age-range'
                                value={age}
                                onChange={(e) => setAge(parseInt(e.target.value))}>
                                <option value='21'>&lt;22</option>
                                <option value='23'>22-26</option>
                                <option value='28'>27-31</option>
                                <option value='33'>32-38</option>
                                <option value='40'>39-44</option>
                                <option value='61'>45-49</option>
                                <option value='51'>&gt;50</option>
                            </select>
                            </div>

                            <div className ='income-elements'>
                            <label className ='income-text' htmlFor='income-bracket'>Income </label>
                            <select
                                className='income-dropdown'
                                id='income-bracket'
                                value={income}
                                onChange={(e) => setIncome(parseInt(e.target.value))}>
                                <option value="9999">&lt;$10,000</option>
                                <option value="10001">$10,000-19,999</option>
                                <option value="20001">$20,000-29,999</option>
                                <option value="30001">$30,000-49,999</option>
                                <option value="50001">$50,000-74,999</option>
                                <option value="75001">&gt;$75,000</option>

                            </select>
                            </div>

                            <div className ='home-elements'>
                            <label className ='home-text' htmlFor='home-dropdown'>Home Ownership</label>
                            <select
                                className='home-dropdown'
                                id='income-bracket'
                                value={homeOwnership}
                                onChange={(e) => setHomeOwnership(parseInt(e.target.value))}>
                                <option value="1">Yes</option>
                                <option value="0">No</option>
                            </select>
                            </div>
                        </div>
                        <div className ='credit-right'>
                                <Button 
                                    label ="Accounts" 
                                    className='accounts-button'
                                    onClick={()=>{
                                        notify("‎  ‎  ‎ <450 Credit: Not Eligible for Account ‎ ‎ ‎ ‎ ‎  " + 
                                            "450 - 600 Credit: Standard Account" +
                                            "‎ ‎ ‎ ‎ " + 
                                            ">600 Credit: Premium Account")
                                    }}
                                />
                            <Button 
                                label ='Generate' 
                                className='generate-report-button'
                                onClick={(e)=>{
                                    fetch("http://localhost:18080/api/check-credit",{
                                        method: 'POST',
                                        headers: {'Content-type': 'application/json'},
                                        body: JSON.stringify({
                                            username:username,
                                            age:age,
                                            income:income,
                                            home:homeOwnership
                                        })
                                    })
                                    .then(res => res.json())
                                    .then(data =>{
                                        if(data.creditScore <450){ 
                                            notify(`${data.creditScore} Credit Points: You are Ineligible for an account.`)
                                        }else if(data.creditScore <600)
                                            notify(`${data.creditScore} Credit Points: Eligible for a Standard Account!`)
                                        else
                                            notify(`${data.creditScore} Credit Points: Eligible for a Premium Account!`) 
                                    })
                                    .catch(err => notify("Generation failed: " + err.message))
                                }}
                            />
                        </div>
                    </div>
                </div>
            </div>
        </>
    )
}

export default Dashboard