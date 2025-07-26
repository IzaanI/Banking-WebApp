// InputCredential.jsx
function InputCredential({ label, type, className = 'defaultCredential', value, onChange }) {
  return (
    <input
      type={type || (label.toLowerCase() === 'password' ? 'password' : 'text')}
      className={className}
      placeholder={label}
      value={value}
      onChange={onChange}
    />
  );
}

export default InputCredential;
