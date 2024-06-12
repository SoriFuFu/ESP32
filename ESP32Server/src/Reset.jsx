import React, { useState, useEffect } from 'react';
import { Card, Form, Button, Row, Col, Spinner } from 'react-bootstrap';
import { GrPowerReset } from "react-icons/gr";
import { MdOutlineLockReset } from "react-icons/md";
import { showErrorAlert, showSuccessAlert } from './alerts';

const  Reset = ({webSocket}) => {

const handleReset = () => {
    if (webSocket) {
        const message = { action: 'RESET' };
        webSocket.send(JSON.stringify(message));
        showSuccessAlert('Dispositivo reiniciado');
    }
    else {
        showErrorAlert('Error al reiniciar el dispositivo');
    }
}

    return (
        <Row>
            <Col md={12}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">Reset</h5>
                    </Card.Header>
                    <Card.Body>
                        <Form.Group as={Row} className="mb-3">
                                <Col md={12} className='d-flex justify-content-between align-items-center'>
                                <Button variant="primary" className='ms-1 me-2' style={{ width: 'calc(50% - 8px)' }} onClick={() => handleReset()}> <GrPowerReset className='me-2' /> Reiniciar</Button>
   
                                    <Button variant="secondary" className='me-1' style={{ width: 'calc(50% - 8px)' }}> <MdOutlineLockReset  className='me-2' /> Restablecer</Button>
                                </Col>

                        </Form.Group>
                    </Card.Body>
                </Card>
            </Col>
        </Row>
    );
}

export default Reset;