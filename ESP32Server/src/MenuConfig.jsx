import { Nav } from 'react-bootstrap';
import { Link } from 'react-router-dom';
import { BsInfoLg, BsWifi } from "react-icons/bs";
import { TbAccessPoint, TbAccessPointOff  } from "react-icons/tb";
import { MdLockReset } from "react-icons/md";

import { IoIosSwitch } from "react-icons/io";
const MenuConfig = ({ wifiEnabled, apEnabled }) => {
    return (
        <Nav justify variant="tabs" defaultActiveKey="/config/info" className="mb-3">
            <Nav.Item>
                <Nav.Link as={Link} to="/config/info" className='d-flex justify-content-center align-items-center'><BsInfoLg className='me-2' />Información</Nav.Link>
            </Nav.Item>
            <Nav.Item>
                <Nav.Link as={Link} to="/config/wifi" className='d-flex justify-content-center align-items-center'><BsWifi className='me-2'/>  WiFi</Nav.Link>
            </Nav.Item>
            <Nav.Item>
                <Nav.Link as={Link} to="/config/ap" className='d-flex justify-content-center align-items-center'> <TbAccessPoint className='me-2'/>AP</Nav.Link>
            </Nav.Item>
            <Nav.Item>
                <Nav.Link as={Link} to="/config/relay" className='d-flex justify-content-center align-items-center'> <IoIosSwitch className='me-2'/>Relés</Nav.Link>

            </Nav.Item>
            <Nav.Item>
                <Nav.Link as={Link} to="/config/reset" className='d-flex justify-content-center align-items-center'> <MdLockReset className='me-2'/>Reset</Nav.Link>
            </Nav.Item>
        </Nav>
    );
}

export default MenuConfig;

