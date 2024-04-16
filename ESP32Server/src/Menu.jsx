import React from 'react';
import { Container, Row, Col } from 'react-bootstrap';
import { Link } from 'react-router-dom';
import { BsHouseDoor, BsGear } from 'react-icons/bs';
import './Menu.css'; // Estilos personalizados para el menú

const Menu = () => {
    return (
        <footer className="footer">
            <Container fluid className="dashboard-container">
            <Row>
                <Col md={12}>
                <div className="d-flex justify-content-center align-self-center ">
                    <Link to="/panel" className="menu-link me-3">
                        <BsHouseDoor className="menu-icon" />
                        
                    </Link>
                    <Link to="/config" className="menu-link">
                        <BsGear className="menu-icon" />
                        
                    </Link>
                </div>
                </Col>
            </Row>
            </Container>
        </footer>
    );
}

export default Menu;

