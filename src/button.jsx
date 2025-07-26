

function Button({label, onClick, tooltip, className = "defaultButton"}){
    
    return(
        <>
            <button className = {className} onClick = {onClick}>
                {label}
            </button>
            {/*tooltip && <span className="tooltip3">{tooltip}</span>*/}
        </>
    );
}

export default Button