import React from 'react';
import { Container, Row, Col } from 'react-bootstrap';
import { Link } from 'react-router-dom';
import { BsHouseDoor, BsGear, BsWifi, BsWifiOff } from 'react-icons/bs';
import { TbAccessPoint, TbAccessPointOff  } from "react-icons/tb";
import './Menu.css'; // Estilos personalizados para el menú

const Menu = ({ wifiEnabled, apEnabled }) => {
    const currentTime = new Date().toLocaleTimeString(); // Obtiene la hora actual
   

    return (
        <footer className="footer">
            <Container fluid className="dashboard-container">
                <Row>
                    <Col md={12}>
                        <div className="d-flex justify-content-between align-items-center">
                            <div>
                                <Link to="/panel" className="menu-link ms-3 me-3">
                                    <BsHouseDoor className="menu-icon" />
                                </Link>
                                <Link to="/config" className="menu-link ms-3">
                                    <BsGear className="menu-icon" />
                                </Link>
                            </div>
                            <div className="d-flex align-items-center me-3">
                                <span className="me-3">{}</span>
                                <span className="me-3">{currentTime}</span> {/* Muestra la hora */}
                                {wifiEnabled ? <BsWifi className="menu-icon me-3" /> : <BsWifiOff className="menu-icon me-3" />}
                                {/* Icono de WiFi */}
                                {/* Icono de modo AP */}
                                {apEnabled ? <TbAccessPoint className="menu-icon" /> : <TbAccessPointOff className="menu-icon" />}
                            </div>
                        </div>
                    </Col>
                </Row>
            </Container>
        </footer>
    );
}

export default Menu;


