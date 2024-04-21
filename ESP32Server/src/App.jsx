import React, { useEffect, useState } from 'react';
import 'bootstrap/dist/css/bootstrap.min.css';
import { Container } from 'react-bootstrap';
import { BrowserRouter as Router, Routes, Route, Navigate, useLocation } from 'react-router-dom';
import './App.css';
import Menu from './Menu';
import Panel from './Panel';
import Config from './Config';

const App = () => {
    return (
        <Router>
            <AppContent />
        </Router>
    );
}

const AppContent = () => {
    // Obtiene la ubicación actual utilizando useLocation de react-router-dom
    const location = useLocation();
    // Almacena la última ruta visitada en el localStorage
    useEffect(() => {
        localStorage.setItem('lastVisitedRoute', location.pathname);
    }, [location]);

    // Obtiene la última ruta visitada almacenada en el localStorage
    const lastVisitedRoute = localStorage.getItem('lastVisitedRoute');

    // Renderiza las rutas
    return (
        <div>
            <Menu />
            <Container fluid className="content">
                <Routes>
                    <Route path="/panel" element={<Panel />} />
                    <Route path="/config" element={<Config />} />
                    {/* Redirige a la última ruta visitada almacenada en localStorage */}
                    <Route path="*" element={<Navigate to={lastVisitedRoute || '/panel'} replace />} />
                </Routes>
            </Container>
        </div>
    );
}

export default App;

