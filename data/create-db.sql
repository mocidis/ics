create table parameter (name VARCHAR(20), value VARCHAR(50));
insert into parameter values ('csu_transport', 'udp');
insert into parameter values ('csu_ip', '127.0.0.1');
insert into parameter values ('csu_port', '5060');

create table class (id INTEGER, operation, class_name);
insert into class values(0, '', 'SIP');
insert into class values(1, '', 'TEL');
insert into class values(2, '', 'RADIO');
insert into class values(3, '', 'OMU');

create table entity (id INTEGER, name, class_id INTEGER, access_info VARCHAR(100), priority INTEGER);
