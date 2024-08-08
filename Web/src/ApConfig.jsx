// ApConfig.jsx
import React, { useState, useEffect } from 'react';
import { Card, Form, Button, Row, Col, Spinner } from 'react-bootstrap';
import { BsSearchHeart, BsEye, BsEyeSlash } from "react-icons/bs";
import { FaUpload } from "react-icons/fa";
import { showErrorAlert, showSuccessAlert } from './alerts';

const ApConfig = ({ apConfig, wifiStatus, updateApConfig, webSocket }) => {

    const [showApPassword, setShowApPassword] = useState(false);

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
                    let message = { action: 'SETAPCONFIG', active: true };
                    webSocket.send(JSON.stringify(message));
                    updateApConfig("active", true);
                    updateApConfig("status", true);
                } else {
                    console.log('WebSocket no está inicializado');
                }
                break;
            case false:
                if (webSocket) {
                    let message = { action: 'SETAPCONFIG', active: false };
                    webSocket.send(JSON.stringify(message));
                    updateApConfig("active", false);
                    updateApConfig("status", false);
                } else {
                    console.log('WebSocket no está inicializado');
                }
                break;
            default:

        }
    };

    const handleUpdateApConfig = (object, value) => {
        updateApConfig(object, value);
        if (webSocket) {
            let message = { action: 'SETAPCONFIG', [object]: value };
            webSocket.send(JSON.stringify(message));
        } else {
            console.log('WebSocket no está inicializado');
        }
    }

    return (
        <Row>
            <Col md={12}>
                            {/* Sección de Configuración AP */}
                            <Card className="mb-3">
                                <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                                    <h5 className="mb-0">Modo AP</h5>
                                    <Form.Switch className="float-end" checked={apConfig.active} onChange={(e) => handleApEnabledChange(e.target.checked)} />
                                </Card.Header>
                                <Card.Body hidden={!apConfig.active}>
                                    <Form.Group className="mb-3">
                                        <Form.Label >SSID:</Form.Label>

                                        <Form.Control type="text" value={apConfig.ssid} onChange={(e) => handleUpdateApConfig("ssid", e.target.value) } disabled />

                                    </Form.Group>
                                    <Form.Group className="mb-3" >
                                        <Form.Label>Contraseña:</Form.Label>
                                        <div className="d-flex flex-row align-items-center">
                                            <Form.Control type={showApPassword ? "text" : "password"} value={apConfig.password} onChange={(e) => handleUpdateApConfig("password", e.target.value) } disabled />

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
