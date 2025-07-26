
function HandleInputFields({val}){
    // Allow empty string
    if (val === "") {
        setMoveAmount("");
        return;
    }

    // Allow valid float-in-progress values
    const floatRegex = /^(\d+)?(\.)?(\d*)?$/;

    if (floatRegex.test(val)) {
        setMoveAmount(val);
    }
}

export default HandleInputFields