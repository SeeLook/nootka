
#include <music/tnote.h>
#include <tinitcorelib.h>
#include <QDebug>

int main(int argc, char *argv[])
{
	Tnote note(1, 0, 0);
	qDebug() << note.toText() << Tglob::glob();
	
}
