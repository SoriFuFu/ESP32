import React from 'react';
import { Card, Row, Col } from 'react-bootstrap';

const InfoConfig = ({ deviceInfo, wifiConfig, apConfig, relay }) => {

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
                            <Card.Title>Información del equipo</Card.Title>
                            <Card.Text><strong>Nombre:</strong> {deviceInfo.name}</Card.Text>
                            <Card.Text><strong>Número de serie:</strong> {deviceInfo.serialNumber}</Card.Text>
                            <Card.Text><strong>Versión de firmware:</strong> {deviceInfo.firmwareVersion}</Card.Text>
                            <Card.Text><strong>Última fecha de reinicio:</strong> {deviceInfo.lastReboot}</Card.Text>
                            {/* Agrega más detalles de la información del equipo aquí */}
                        </Card.Body>
                    </Card>
                </Col>

                <Col md={6} className='mb-3'>
                    <Card>
                        <Card.Body>
                            <Card.Title>Configuración de WiFi</Card.Title>
                            
                                <Card.Text><strong>Estado:</strong> {renderStatus(wifiConfig.status)}</Card.Text>
                                <Card.Text><strong>SSID:</strong> {wifiConfig.ssid}</Card.Text>
                                <Card.Text><strong>Contraseña:</strong> {wifiConfig.password ? wifiConfig.password : 'Sin contraseña'}</Card.Text>

                                <Card.Text><strong>IP:</strong> {wifiConfig.ip}</Card.Text>
                                <Card.Text><strong>Subred</strong> {wifiConfig.subnet}</Card.Text>
                                <Card.Text><strong>Puerta de enlace:</strong> {wifiConfig.gateway}</Card.Text>
                                {/* Agrega más detalles de la configuración de WiFi aquí */}
                          
                        </Card.Body>
                    </Card>
                </Col>
            
                <Col md={6} className='mb-3'>
                    <Card>
                        <Card.Body>
                            <Card.Title>Configuración de AP</Card.Title>
                        
                                <Card.Text><strong>Estado:</strong> {renderStatus(apConfig.status)}</Card.Text>
                                <Card.Text><strong>SSID:</strong> {apConfig.ssid}</Card.Text>
                                <Card.Text><strong>Contraseña:</strong> {apConfig.password}</Card.Text>
                                {/* Agrega más detalles de la configuración de AP aquí */}
                            
                        </Card.Body>
                    </Card>
                </Col>
            </Row>
            <Row className='mb-3' >
                {Object.keys(relay).map((key) => {
                    const individualRelay = relay[key];
                    return (
                        <Col key={key} md={6} className='mb-2'>
                            <Card>
                                <Card.Body>
                                    <Card.Title>Configuración de {key}</Card.Title>
                                    
                                        <Card.Text><strong>Estado:</strong> {renderStatus(individualRelay.active)}</Card.Text>
                                        <Card.Text><strong>Nombre: </strong> {individualRelay.name}</Card.Text>
                                        {/* Agrega más detalles de la configuración de relé aquí */}
                                   
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
