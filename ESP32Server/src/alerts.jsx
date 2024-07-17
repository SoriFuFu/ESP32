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

export const resetAlert = (message, ip) => {
  let timerInterval;
  Swal.fire({
    title: "Reiniciando equipo",
    html: "La ventana se cerrará en <b></b> segundos.",
    timer: 10000,
    timerProgressBar: true,
    didOpen: () => {
      Swal.showLoading();
      const timer = Swal.getHtmlContainer().querySelector("b");
      timerInterval = setInterval(() => {
        const secondsLeft = Math.ceil(Swal.getTimerLeft() / 1000);
        timer.textContent = `${secondsLeft}`;
      }, 100);
    },
    willClose: () => {
      clearInterval(timerInterval);
    }
  }).then((result) => {
    if (result.dismiss === Swal.DismissReason.timer) {
      console.log("Se ha reiniciado el equipo");
      let url = 'http://' + ip;
      console.log(url);
      window.location.href = url ; // Reemplaza con la URL a la que deseas redirigir
    }
  });
  
}