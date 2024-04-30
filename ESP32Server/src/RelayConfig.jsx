import React, { useState, useEffect } from 'react';
import { Card, Form, Button, Row, Col, Spinner } from 'react-bootstrap';
import { BsSearchHeart, BsEye, BsEyeSlash } from "react-icons/bs";
import { FaUpload } from "react-icons/fa";
import { showErrorAlert, showSuccessAlert } from './alerts';

const Relay = ({ relay, setK1EnabledConfig, setK2EnabledConfig, setK3EnabledConfig, setK4EnabledConfig,  webSocket }) => {
    const [K1Enabled, setK1Enabled] = useState(relay.K1.active);
    const [K1Name, setK1Name] = useState(relay.K1.name);
    const [K2Enabled, setK2Enabled] = useState(relay.K2.active);
    const [K2Name, setK2Name] = useState(relay.K2.name);
    const [K3Enabled, setK3Enabled] = useState(relay.K3.active);
    const [K3Name, setK3Name] = useState(relay.K3.name);
    const [K4Enabled, setK4Enabled] = useState(relay.K4.active);
    const [K4Name, setK4Name] = useState(relay.K4.name);

    const updateRelayConfig = (relayName, status) => {
        if (relayName === 'K1') {
            setK1Enabled(status);
            setK1EnabledConfig(status);
            if (webSocket) {
                let message = { action: 'setRelayActive', relay: "K1", K1Active: status};
                webSocket.send(JSON.stringify(message));
            } else {
                console.log('WebSocket no está inicializado');
            }
        } else if (relayName === 'K2') {
            setK2Enabled(status);
            setK2EnabledConfig(status);
            if (webSocket) {
                let message = { action: 'setRelayActive', relay: "K2", K2Active: status};
                webSocket.send(JSON.stringify(message));
            } else {
                console.log('WebSocket no está inicializado');
            }
            
        } else if (relayName === 'K3') {
            setK3Enabled(status);
            setK3EnabledConfig(status);
            if (webSocket) {
                let message = { action: 'setRelayActive', relay: "K3", K3Active: status};
                webSocket.send(JSON.stringify(message));
            }else {
                console.log('WebSocket no está inicializado');
            }
            
        } else if (relayName === 'K4') {
            setK4Enabled(status);
            setK4EnabledConfig(status);
            if (webSocket) {
                let message = { action: 'setRelayActive', relay: "K4", K4Active: status};
                webSocket.send(JSON.stringify(message));
            } else {
                console.log('WebSocket no está inicializado');
            }
        }
    }

    const setRelayName = (relay) => {
        if (webSocket) {
            if (relay === 1) {
                let message = { action: 'setRelayName', relay: "K1", relayName: K1Name};
                webSocket.send(JSON.stringify(message));
            } else if (relay === 2) {
                let message = { action: 'setRelayName', relay: "K2",  relayName: K2Name };
                webSocket.send(JSON.stringify(message));
            } else if (relay === 3) {
                let message = { action: 'setRelayName', relay: "K3", relayName: K3Name };
                webSocket.send(JSON.stringify(message));
            } else if (relay === 4) {
                let message = { action: 'setRelayName', relay: "K4", relayName: K4Name };
                webSocket.send(JSON.stringify(message));
            }

        } else {
            console.log('WebSocket no está inicializado');
        }
    }

    return (
        <Row>
            <Col md={6}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">K1</h5>
                        <Form.Switch className="float-end" checked={K1Enabled} onChange={(e) => updateRelayConfig("K1",e.target.checked)} />
                    </Card.Header>
                    <Card.Body hidden={!K1Enabled}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Nombre</Form.Label>
                            <Col sm={9}>
                                <Form.Control type="text" value={K1Name} onChange={(e) => setK1Name(e.target.value)} />
                            </Col>
                        </Form.Group>

                        <Button variant="primary" onClick={() => setRelayName(1)} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                    </Card.Body>
                </Card>
            </Col>

            <Col md={6}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">K2</h5>
                        <Form.Switch className="float-end" checked={K2Enabled} onChange={(e) => updateRelayConfig("K2",e.target.checked)} />
                    </Card.Header>
                    <Card.Body hidden={!K2Enabled}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Nombre</Form.Label>
                            <Col sm={9}>
                                <Form.Control type="text" value={K2Name} onChange={(e) => setK2Name(e.target.value)} />
                            </Col>
                        </Form.Group>

                        <Button variant="primary" onClick={() => setRelayName(2)} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                    </Card.Body>
                </Card>
            </Col>

            <Col md={6}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">K3</h5>
                        <Form.Switch className="float-end" checked={K3Enabled} onChange={(e) => updateRelayConfig("K3",e.target.checked)} />
                    </Card.Header>
                    <Card.Body hidden={!K3Enabled}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Nombre</Form.Label>
                            <Col sm={9}>
                                <Form.Control type="text" value={K3Name} onChange={(e) => setK3Name(e.target.value)} />
                            </Col>
                        </Form.Group>

                        <Button variant="primary" onClick={() => setRelayName(3)} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                    </Card.Body>
                </Card>
            </Col>

            <Col md={6}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">K4</h5>
                        <Form.Switch className="float-end" checked={K4Enabled} onChange={(e) => updateRelayConfig("K4",e.target.checked)} />
                    </Card.Header>
                    <Card.Body hidden={!K4Enabled}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Nombre</Form.Label>
                            <Col sm={9}>
                                <Form.Control type="text" value={K4Name} onChange={(e) => setK4Name(e.target.value)} />
                            </Col>
                        </Form.Group>

                        <Button variant="primary" onClick={() => setRelayName(4)} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                    </Card.Body>
                </Card>
            </Col>
        </Row> 

    );
}

export default Relay;
