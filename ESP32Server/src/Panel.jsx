import React from 'react';
import { Container, Row, Col } from 'react-bootstrap';
import RelayCard from './RelayCard';

const Panel = ({ Relay }) => {
    console.log('Panel', Relay);
    return (
        <div>
            <Row className="mb-3">
                <Col>
                    <h2>Panel</h2>
                </Col>
            </Row>
            <Container>
                <Row>
                    {Object.keys(Relay).map((key) => (
                        // Verificar si el relevo está activo antes de mostrar el componente
                        Relay[key].active && (
                            <Col md={6} key={key} className='mb-2'>
                                {/* Pasa cada objeto de relevo y su clave como prop a RelayCard */}
                                <RelayCard relay={Relay[key]} relayKey={key} />
                            </Col>
                        )
                    ))}
                </Row>
            </Container>
        </div>
    );
}

export default Panel;
