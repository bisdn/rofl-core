#!/usr/bin/python

from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm.exc import NoResultFound
from sqlalchemy.orm import sessionmaker
from sqlalchemy import create_engine
from sqlalchemy import *


ports = { 'eth1': '00:11:11:11:11:11', 'eth2': '00:22:22:22:22:22', 'eth3': '00:33:33:33:33:33', 'eth4': '00:44:44:44:44:44' }
vmports = { 'eth1': 'vnet1', 'eth2':'vnet2', 'eth3':'vnet3', 'eth4':'vnet4' }


engine = create_engine('sqlite:///datapath.db', echo=True)
Base = declarative_base()


class Psi(Base):
    __tablename__ = 'psi'
    
    id = Column(Integer, Sequence('psi_id_seq'), primary_key = True)  
    name = Column(String(64), nullable=False)
    
    def __init__(self, name):
        self.name = name
        
    def __repr__(self):
        return "<PSI('%s')>" % (self.name)


class Lsi(Base):
    __tablename__ = 'lsi'
    
    id = Column(Integer, Sequence('lsi_id_seq'), primary_key = True)
    dpid = Column(Integer, nullable = False)
    psi_id = Column(Integer, ForeignKey("psi.id"), nullable = False)

    def __init__(self, dpid, psi_id):
        self.dpid = dpid
        self.psi_id = psi_id
        
    def __repr__(self):
        return "<LSI('%s', '%s')>" % (self.dpid, self.psi_id)
    
    
class Port(Base):
    __tablename__ = 'port'
    
    id = Column(Integer, Sequence('port_id_seq'), primary_key = True)
    name = Column(String(16), nullable = False)
    hwaddr = Column(String(18), nullable = False)
    lsi_id = Column(Integer, ForeignKey("lsi.id"), nullable = False)
    
    def __init__(self, name, hwaddr, lsi_id):
        self.name = name
        self.hwaddr = hwaddr
        self.lsi_id = lsi_id
    
    def __repr__(self):
        return "<PORT('%s', '%s', '%s')>" % (self.name, self.hwaddr, self.lsi_id)
    
    
class VmPort(Base):
    __tablename__ = 'vmport'
    
    id = Column(Integer, Sequence('vmport_seq_id'), primary_key = True)
    name = Column(String(16), nullable = False)
    port_id = Column(Integer, ForeignKey("port.id"), nullable = False, unique = True)
    
    def __init__(self, name, port_id):
        self.name = name
        self.port_id = port_id
    
    def __repr__(self):
        return "<VMPORT('%s', '%s')>" % (self.name, self.port_id)
     
    
    
Base.metadata.create_all(engine) 



    
def main():
    Session = sessionmaker(bind=engine)
    session = Session()
    
    try:
        session.query(Psi).filter(Psi.name == 'my PSI').one()
    except NoResultFound, e:
        session.add(Psi("my PSI"))
        session.commit()
        
    my_psi = session.query(Psi).filter(Psi.name == 'my PSI').one()
    
    
    try:
        session.query(Lsi).filter(Lsi.dpid == '1544').one()
    except NoResultFound, e:    
        session.add(Lsi('1544', my_psi.id))
        session.commit()
        
    my_lsi = session.query(Lsi).filter(Lsi.dpid == '1544').one()
    
    
    for name in sorted(ports.iterkeys()):
        try:
            session.query(Port).filter(Port.name == name).one()
        except NoResultFound, e:
            session.add(Port(name, ports[name], my_lsi.id))
    session.commit()
    
    
    for name in sorted(vmports.iterkeys()):
        try:
            session.query(VmPort).join(Port).join(Lsi).join(Psi).\
                    filter(Port.name==name).one()
        except NoResultFound, e:
            session.add(VmPort(vmports[name], session.query(Port).join(Lsi).join(Psi).\
                    filter(Port.name==name).one().id))
    session.commit()
    
    
    
    
    
if __name__ == "__main__":
    main()
    
    
    
    