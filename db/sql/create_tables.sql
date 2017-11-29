create table Patient
(
    pat_id          int         not null,
    pat_firstname   char(255)   not null,
    pat_surname     char(255)   not null,
    pat_dob         datetime    not null
);

alter table Patient add primary key (pat_id);
