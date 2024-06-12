import React, { useState, useEffect } from 'react';
import { Card, Form, Button, Row, Col, Spinner } from 'react-bootstrap';
import { BsSearchHeart, BsEye, BsEyeSlash } from "react-icons/bs";
import { FaUpload } from "react-icons/fa";
import { showErrorAlert, showSuccessAlert } from './alerts';

const Relay = ({ relay, updateRelay, webSocket }) => {
    const [K1Name, setK1Name] = useState(relay.K1.name);
    const [K2Name, setK2Name] = useState(relay.K2.name);

    const updateRelayConfig = (relayID, status) => {
        updateRelay(relayID, 'enabled', status);
        updateRelay(relayID, 'active', status);
        if (webSocket) {
            let Kactive = relayID === "K1" ? "K1Active" : "K2Active";
            let message = { action: 'SETRELAYACTIVE', relay: relayID, KActive: status };
            webSocket.send(JSON.stringify(message));
        } else {
            console.log('WebSocket no está inicializado');
        }

    }

    const setRelayName = (relayID) => {
        if (webSocket) {
            let message = { action: 'SETRELAYNAME', relay: relayID, relayName: relayID === "K1" ? K1Name : K2Name };
            webSocket.send(JSON.stringify(message));
            updateRelay(relayID, "name", relayID === "K1" ? K1Name : K2Name);

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
                        <Form.Switch className="float-end" checked={relay.K1.active} onChange={(e) => updateRelayConfig("K1", e.target.checked)} />
                    </Card.Header>
                    <Card.Body hidden={!relay.K1.active}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Nombre</Form.Label>
                            <Col sm={9}>
                                <Form.Control type="text" value={K1Name} onChange={(e) => setK1Name(e.target.value)} />
                            </Col>
                        </Form.Group>

                        <Button variant="primary" onClick={() => setRelayName("K1")} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                    </Card.Body>
                </Card>
            </Col>

            <Col md={6}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">K2</h5>
                        <Form.Switch className="float-end" checked={relay.K2.active} onChange={(e) => updateRelayConfig("K2", e.target.checked)} />
                    </Card.Header>
                    <Card.Body hidden={!relay.K2.active}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Nombre</Form.Label>
                            <Col sm={9}>
                                <Form.Control type="text" value={K2Name} onChange={(e) => setK2Name(e.target.value)} />
                            </Col>
                        </Form.Group>

                        <Button variant="primary" onClick={() => setRelayName("K2")} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                    </Card.Body>
                </Card>
            </Col>

        </Row>

    );
}

export default Relay;
