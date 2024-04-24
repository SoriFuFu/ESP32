import React, { useState, useEffect } from 'react';
import { Card, Form, Button, Row, Col, Spinner } from 'react-bootstrap';
import { BsSearchHeart, BsEye, BsEyeSlash } from "react-icons/bs";
import { FaUpload } from "react-icons/fa";
import { showErrorAlert, showSuccessAlert } from './alerts';

const Relay = ({ relay, webSocket }) => {
    const [K1Enabled, setK1Enabled] = useState(relay.K1.active);
    const [K1Name, setK1Name] = useState(relay.K1.name);
    const [K1Mode, setK1Mode] = useState(relay.K1.mode);
    const [K2Enabled, setK2Enabled] = useState(relay.K2.active);
    const [K2Name, setK2Name] = useState(relay.K2.name);
    const [K2Mode, setK2Mode] = useState(relay.K2.mode);
    const [K3Enabled, setK3Enabled] = useState(relay.K3.active);
    const [K3Name, setK3Name] = useState(relay.K3.name);
    const [K3Mode, setK3Mode] = useState(relay.K3.mode);
    const [K4Enabled, setK4Enabled] = useState(relay.K4.active);
    const [K4Name, setK4Name] = useState(relay.K4.name);
    const [K4Mode, setK4Mode] = useState(relay.K4.mode);

    const setRelayConfig = (relay) => {
        if (webSocket) {
            if (relay === 1) {
                console.log('K1');
                let message = { action: 'setRelayConfig', relay: relay, K1Active: true, relayName: K1Name, relayMode: K1Mode };
                webSocket.send(JSON.stringify(message));
            } else if (relay === 2) {
                let message = { action: 'setRelayConfig', relay: relay, K2Active: true, relayName: K2Name, relayMode: K2Mode };
                webSocket.send(JSON.stringify(message));
            } else if (relay === 3) {
                let message = { action: 'setRelayConfig', relay: relay, K3Active: true, relayName: K3Name, relayMode: K3Mode };
                webSocket.send(JSON.stringify(message));
            } else if (relay === 4) {
                let message = { action: 'setRelayConfig', relay: relay, K4Active: true, relayName: K4Name, relayMode: K4Mode };
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
                        <Form.Switch className="float-end" checked={K1Enabled} onChange={(e) => setK1Enabled(e.target.checked)} />
                    </Card.Header>
                    <Card.Body hidden={!K1Enabled}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Nombre</Form.Label>
                            <Col sm={9}>
                                <Form.Control type="text" value={K1Name} onChange={(e) => setK1Name(e.target.value)} />
                            </Col>
                        </Form.Group>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                            <Col sm={9}>
                                <Form.Select >
                                    <option value="1">Temporizador regresivo</option>
                                    <option value="2">Programación diaria</option>
                                    <option value="3">Enclavamiento</option>
                                    <option value="4">Activación por Eventos Externos</option>
                                </Form.Select>
                            </Col>
                        </Form.Group>
                        <Button variant="primary" onClick={() => setRelayConfig(1)} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                    </Card.Body>
                </Card>
            </Col>

            <Col md={6}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">K2</h5>
                        <Form.Switch className="float-end" checked={K2Enabled} onChange={(e) => setK2Enabled(e.target.checked)} />
                    </Card.Header>
                    <Card.Body hidden={!K2Enabled}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Nombre</Form.Label>
                            <Col sm={9}>
                                <Form.Control type="text" value={K2Name} disabled={!K2Enabled} />
                            </Col>
                        </Form.Group>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                            <Col sm={9}>
                                <Form.Select disabled={!K2Enabled}>
                                    <option value="1">Temporizador regresivo</option>
                                    <option value="2">Programación diaria</option>
                                    <option value="3">Enclavamiento</option>
                                    <option value="4">Activación por Eventos Externos</option>
                                </Form.Select>
                            </Col>
                        </Form.Group>
                        <Button variant="primary" onClick={() => setRelayConfig(2)} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                    </Card.Body>
                </Card>
            </Col>

            <Col md={6}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">K3</h5>
                        <Form.Switch className="float-end" checked={K3Enabled} onChange={(e) => setK3Enabled(e.target.checked)} />
                    </Card.Header>
                    <Card.Body hidden={!K3Enabled}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Nombre</Form.Label>
                            <Col sm={9}>
                                <Form.Control type="text" value={K3Name} disabled={!K3Enabled} />
                            </Col>
                        </Form.Group>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                            <Col sm={9}>
                                <Form.Select disabled={!K3Enabled}>
                                    <option value="1">Temporizador regresivo</option>
                                    <option value="2">Programación diaria</option>
                                    <option value="3">Enclavamiento</option>
                                    <option value="4">Activación por Eventos Externos</option>
                                </Form.Select>
                            </Col>
                        </Form.Group>
                        <Button variant="primary" onClick={() => setRelayConfig(3)} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                    </Card.Body>
                </Card>
            </Col>

            <Col md={6}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">K4</h5>
                        <Form.Switch className="float-end" checked={K4Enabled} onChange={(e) => setK4Enabled(e.target.checked)} />
                    </Card.Header>
                    <Card.Body hidden={!K4Enabled}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Nombre</Form.Label>
                            <Col sm={9}>
                                <Form.Control type="text" value={K4Name} disabled={!K4Enabled} />
                            </Col>
                        </Form.Group>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                            <Col sm={9}>
                                <Form.Select disabled={!K4Enabled}>
                                    <option value="1">Temporizador regresivo</option>
                                    <option value="2">Programación diaria</option>
                                    <option value="3">Enclavamiento</option>
                                    <option value="4">Activación por Eventos Externos</option>
                                </Form.Select>
                            </Col>
                        </Form.Group>
                        <Button variant="primary" onClick={() => setRelayConfig(4)} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                    </Card.Body>
                </Card>
            </Col>
        </Row> 

    );
}

export default Relay;
