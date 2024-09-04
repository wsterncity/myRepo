#include <IQCore/igQtMainWindow.h>
#include <QApplication>

#if __linux__
#include <qtextcodec.h>
#else
#include <QtCore/Qtextcodec.h>
#endif

#include <IQCore/iGameFileDialog.h>
int main(int argc, char* argv[]) {
    //	::testing::InitGoogleTest(&argc, argv);
    //	// 运行所有测试
    //	return RUN_ALL_TESTS();

    QCoreApplication::setAttribute(
            Qt::AA_EnableHighDpiScaling); // 窗口高分辨率支持
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps); // 图标高分辨率支持
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication a(argc, argv);
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setSamples(16);
    format.setDepthBufferSize(32);
    //format.setStencilBufferSize (16);
    format.setVersion(4, 6); //Mac set to format.setVersion(4, 1);
    //format.setProfile (QSurfaceFormat::CompatibilityProfile);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    a.processEvents();
    QTextCodec* codec = QTextCodec::codecForName("GBK");
    igQtMainWindow w;
    w.setWindowTitle(codec->toUnicode("iGame-MeshView"));
    w.show();
    w.showMaximized();
    a.exec();

    //    QApplication app(argc, argv);
    //
    ////    iGameFileDialog::getOpenFileNames("Load file", QDir::currentPath(), "ALL FIle(*.obj *.off *.stl *.vtk *.mesh *.pvd *.vts)");
    //    iGameFileDialog fileDialog;
    //    fileDialog.exec();
    return 0;
}
