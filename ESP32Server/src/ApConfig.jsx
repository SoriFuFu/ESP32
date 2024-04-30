// ApConfig.jsx
import React, { useState, useEffect } from 'react';
import { Card, Form, Button, Row, Col, Spinner } from 'react-bootstrap';
import { BsSearchHeart, BsEye, BsEyeSlash } from "react-icons/bs";
import { FaUpload } from "react-icons/fa";
import { showErrorAlert, showSuccessAlert } from './alerts';

const ApConfig = ({ apConfig, wifiStatus, setApStatusConfig, webSocket }) => {
    const [showApPassword, setShowApPassword] = useState(false); // Estado para mostrar u ocultar la contraseña del AP
    const [apEnabled, setApEnabled] = useState(apConfig.active);
    const [apStatus, setApStatus] = useState(apConfig.status); // Estado para el estado de la conexión AP
    const [apName, setApName] = useState(apConfig.ssid);
    const [apPassword, setApPassword] = useState(apConfig.password);

    //ACTIVAR O DESACTIVAR EL MODO AP

    const handleApEnabledChange = (checked) => {
        if (wifiStatus) {
            handleApEnabled(checked);
        } else if (!wifiStatus) {
            showErrorAlert('No puedes desactivar la red AP sin conectar a una red WiFi');
            setApEnabled(true);

        }
    };

    const handleApEnabled = (enabled) => {
        switch (enabled) {
            case true:
                if (webSocket) {
                    let message = { action: 'setApConfig', active: true };
                    webSocket.send(JSON.stringify(message));
                    setApStatus(true);
                    setApEnabled(true);
                    setApStatusConfig(true);
                    

                } else {
                    console.log('WebSocket no está inicializado');
                }
                break;
            case false:
                if (webSocket) {
                    let message = { action: 'setApConfig', active: false };
                    webSocket.send(JSON.stringify(message));
                    setApStatus(false);
                    setApEnabled(false);
                    setApStatusConfig(false);
                   
                } else {
                    console.log('WebSocket no está inicializado');
                }
                break;
            default:

        }
    };

    return (
        <Row>
            <Col md={12}>
                            {/* Sección de Configuración AP */}
                            <Card className="mb-3">
                                <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                                    <h5 className="mb-0">Modo AP</h5>
                                    <Form.Switch className="float-end" checked={apEnabled} onChange={(e) => handleApEnabledChange(e.target.checked)} />
                                </Card.Header>
                                <Card.Body hidden={!apEnabled}>
                                    <Form.Group className="mb-3">
                                        <Form.Label >SSID:</Form.Label>

                                        <Form.Control type="text" value={apName} onChange={(e) => setApName(e.target.value)} disabled />

                                    </Form.Group>
                                    <Form.Group className="mb-3" >
                                        <Form.Label>Contraseña:</Form.Label>
                                        <div className="d-flex flex-row align-items-center">
                                            <Form.Control type={showApPassword ? "text" : "password"} value={apPassword} onChange={(e) => setApPassword(e.target.value)} disabled />

                                            <Button className="ms-2" onClick={() => setShowApPassword(!showApPassword)}>{showApPassword ? <BsEyeSlash /> : <BsEye />}</Button>
                                        </div>
                                    </Form.Group>
                                    {/* <Button variant="primary" onClick={() => setAPConfig()} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button> */}
                                </Card.Body>
                            </Card>
                        </Col>
        </Row>
    );
}

export default ApConfig;
