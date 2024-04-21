// alerts.jsx
import Swal from 'sweetalert2';

export const showErrorAlert = (message) => {
  Swal.fire({
    toast: true,
    icon: 'error',
    title: message,
    position: 'top-end',
    showConfirmButton: false,
    timer: 3000,
  });
};

export const showSuccessAlert = (message) => {
  Swal.fire({
    toast: true,
    icon: 'success',
    title: message,
    position: 'top-end',
    showConfirmButton: false,
    timer: 3000,
  });
};
