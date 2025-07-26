import React, { useState, useEffect } from 'react';

function ToastExample() {
  const [showToast, setShowToast] = useState(false);
  const [fadeOut, setFadeOut] = useState(false);

  const triggerToast = () => {
    setShowToast(true);
    setFadeOut(false); // Reset fadeOut in case it's still true
  };

  useEffect(() => {
    if (showToast) {
      const fadeTimer = setTimeout(() => setFadeOut(true), 2000);     // start fading after 2s
      const hideTimer = setTimeout(() => setShowToast(false), 3000);  // fully hide after 3s

      return () => {
        clearTimeout(fadeTimer);
        clearTimeout(hideTimer);
      };
    }
  }, [showToast]);

  return (
    <div>
      <button onClick={triggerToast}>Trigger Toast</button>

      {showToast && (
        <div className={`custom-toast ${fadeOut ? 'fade-out' : ''}`}>
          ✅ Account successfully created!
        </div>
      )}
    </div>
  );
}

export default ToastExample;
