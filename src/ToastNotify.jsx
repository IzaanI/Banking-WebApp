import { ToastContainer, toast } from 'react-toastify';

function notify (text) {
    toast(text,{
        position: "top-right",
        autoClose: 4000,
        hideProgressBar: false,
        closeOnClick: false,
        pauseOnHover: true,
        draggable: true,
        progress: undefined,
        theme: "dark",
    })
};

export default notify;