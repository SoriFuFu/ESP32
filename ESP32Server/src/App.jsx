import React, { useEffect } from 'react';
import 'bootstrap/dist/css/bootstrap.min.css';
import { Container } from 'react-bootstrap';
import { BrowserRouter as Router, Routes, Route, Navigate } from 'react-router-dom';
import './App.css';
import Menu from './Menu';
import Panel from './Panel';
import Config from './Config';

const App = () => {
      return (
        <Router>
            <div>
                <Menu />
                <Container fluid className="content">
                <Routes>
                    <Route path="/panel" element={<Panel />} />
                    <Route path="/config" element={<Config />} />
                    {/* Redirige a la última ruta visitada almacenada en localStorage */}
                    <Route path="*" element={<Navigate to="/panel" replace />} />
                </Routes>
                </Container>
            </div>
        </Router>
    );
}

export default App;
