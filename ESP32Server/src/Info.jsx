import React from 'react';
import { Card, Row, Col } from 'react-bootstrap';

const InfoConfig = ({ wifiConfig, apConfig, relay }) => {

    const renderStatus = (status) => {
        if (status) {
            return <span style={{ color: 'green' }}>Conectado</span>;
        } else {
            return <span style={{ color: 'red' }}>Desconectado</span>;
        }
    };
    return (
        <>
            <Row>
                <Col>
                    <h1>Información</h1>
                </Col>
            </Row>
            <Row>
                <Col md={6} className='mb-3'>
                    <Card>
                        <Card.Body>
                            <Card.Title>Configuración de WiFi</Card.Title>
                            <Card.Text>
                                <p><strong>Estado:</strong> {renderStatus(wifiConfig.status)}</p>
                                <p><strong>SSID:</strong> {wifiConfig.ssid}</p>
                                <p><strong>Contraseña:</strong> {wifiConfig.password ? wifiConfig.password : 'Sin contraseña'}</p>

                                <p><strong>IP:</strong> {wifiConfig.ip}</p>
                                <p><strong>Subred</strong> {wifiConfig.subnet}</p>
                                <p><strong>Puerta de enlace:</strong> {wifiConfig.gateway}</p>
                                {/* Agrega más detalles de la configuración de WiFi aquí */}
                            </Card.Text>
                        </Card.Body>
                    </Card>
                </Col>
            
                <Col md={6} className='mb-3'>
                    <Card>
                        <Card.Body>
                            <Card.Title>Configuración de AP</Card.Title>
                            <Card.Text>
                                <p><strong>Estado:</strong> {renderStatus(apConfig.status)}</p>
                                <p><strong>SSID:</strong> {apConfig.ssid}</p>
                                <p><strong>Contraseña:</strong> {apConfig.password}</p>
                                {/* Agrega más detalles de la configuración de AP aquí */}
                            </Card.Text>
                        </Card.Body>
                    </Card>
                </Col>
            </Row>
            <Row>
                {Object.keys(relay).map((key) => {
                    const individualRelay = relay[key];
                    return (
                        <Col key={key} md={6} className='mb-2'>
                            <Card>
                                <Card.Body>
                                    <Card.Title>Configuración de {key}</Card.Title>
                                    <Card.Text>
                                        <p><strong>Estado:</strong> {renderStatus(individualRelay.state)}</p>
                                        <p><strong>Nombre: </strong> {individualRelay.name}</p>
                                        {/* Agrega más detalles de la configuración de relé aquí */}
                                    </Card.Text>
                                </Card.Body>
                            </Card>
                        </Col>
                    );
                })}


            </Row>
        </>
    );
}

export default InfoConfig;
