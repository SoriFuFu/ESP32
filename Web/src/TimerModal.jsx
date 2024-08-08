import React, { useState } from 'react';
import { Modal, Form, Row, Col, Button } from 'react-bootstrap';
import { MdTimer } from "react-icons/md";

const TimerModal = ({ relay, relayState, handleModalClose, handleTimeSelection, handleTimerOff }) => {
    const [timerSelected, setTimerSelected] = useState(relayState.timerSelected);
    const handleTimeSelectionModal = () => {
        if (timerSelected > 0) {
            handleTimeSelection(relay, timerSelected);
        } else {
            console.log('Selecciona un tiempo válido');
        }
    };

    return (
        <Modal show={true} onHide={handleModalClose}>
            <Modal.Header closeButton>
                <Modal.Title>Seleccionar Tiempo</Modal.Title>
            </Modal.Header>
            <Modal.Body>
                <Form>
                    <Row className="align-items-center">
                        <Col xs={9} sm={9}>
                            <Form.Control
                                type="range"
                                className='form-range'
                                value={timerSelected}
                                min={0}
                                max={7200000} // Máximo de 2 horas en milisegundos
                                step={60000} // Pasos de 1 segundo
                                onChange={(e) => setTimerSelected(parseInt(e.target.value))}
                            />
                        </Col>
                        <Col xs={3} sm={3}>
                            <Form.Control
                                type="text"
                                readOnly
                                value={`${Math.floor(timerSelected / 3600000).toString().padStart(2, '0')}:${Math.floor((timerSelected % 3600000) / 60000).toString().padStart(2, '0')}`}
                            />
                        </Col>
                    </Row>
                </Form>
            </Modal.Body>
            <Modal.Footer>
                <Button variant="danger" onClick={() => handleTimerOff(relay, false)}>
                    Desactivar
                </Button>
                <Button variant="secondary" onClick={handleModalClose}>
                    Cancelar
                </Button>
                <Button variant="primary" onClick={handleTimeSelectionModal}>
                    Hecho
                </Button>
            </Modal.Footer>
        </Modal>
    );
};

export default TimerModal;

