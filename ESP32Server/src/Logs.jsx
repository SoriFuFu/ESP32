import React, { useState, useEffect } from 'react';
import { Card, Table, Button, Row, Col, Spinner } from 'react-bootstrap';
import { GrPowerReset } from "react-icons/gr";

import { showErrorAlert, showSuccessAlert } from './alerts';

const Log = ({ webSocket, logs }) => {

    const handleSerachLogs = () => {
        if (webSocket) {
            const message = { action: 'GETLOGS' };
            webSocket.send(JSON.stringify(message));
        }
        else {
            showErrorAlert('Error al reiniciar el dispositivo');
        }
    }
    useEffect(() => {
        handleSerachLogs();
    },[]
    );


    return (
        <Row>
            <Col md={12}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">LOGS</h5>
                        <Button variant="primary" className='ms-1 me-2' style={{ width: 'calc(50% - 8px)' }} onClick={() => handleSerachLogs()}> <GrPowerReset className='me-2' /> Recargar</Button>
                    </Card.Header>
                    <Card.Body >
                        {logs ? (
                            <Table striped bordered hover>
                                <thead>
                                    <tr>
                                        <th>#</th>
                                        <th>Date</th>
                                        <th>Error Message</th>
                                    </tr>
                                </thead>
                                <tbody>
                                    {logs.map((log, index) => (
                                        <tr key={index}>
                                            <td>{index + 1}</td>
                                            <td>{log.date}</td>
                                            <td>{log.error}</td>
                                        </tr>
                                    ))}
                                </tbody>
                            </Table>
                        ) : (
                            <p>Loading...</p>
                        )}

                    </Card.Body>
                </Card>
            </Col>

        </Row>

    );
}

export default Log;
