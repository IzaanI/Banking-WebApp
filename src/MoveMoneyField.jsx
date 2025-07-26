
function MoveMoneyField({value, onChange, className, label}){
    return(
        <input
            className = {className} 
            placeholder={label} 
            value = {value}
            onChange = {onChange}
        />
    )
}

export default MoveMoneyField