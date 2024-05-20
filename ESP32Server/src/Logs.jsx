import React, { useState, useEffect } from 'react';
import { Card, Form, Button, Row, Col, Spinner } from 'react-bootstrap';

import { showErrorAlert, showSuccessAlert } from './alerts';

const Log = () => {

    const [data, setData] = useState(null);

    useEffect(() => {
        // Hacer una solicitud GET al archivo JSON
        fetch('/Logs.json')
            .then(response => {
                // Verificar si la respuesta es exitosa
                if (!response.ok) {
                    throw new Error('Network response was not ok');
                }
                return response.json(); // Parsear la respuesta JSON
            })
            .then(data => {
                setData(data); // Guardar los datos en el estado
            })
            .catch(error => {
                console.error('Error fetching the JSON:', error);
            });
    }, []); // El array vacío [] asegura que este efecto se ejecute solo una vez

    return (
        <Row>
            <Col md={12}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">LOGS</h5>
                    </Card.Header>
                    <Card.Body >
                        {data ? (
                            <pre>{JSON.stringify(data, null, 2)}</pre> // Mostrar los datos JSON en el componente
                        ) : (
                            <p>Loading...</p> // Mostrar un mensaje de carga mientras se obtiene el JSON
                        )}
                    </Card.Body>
                </Card>
            </Col>

        </Row>

    );
}

export default Log;
