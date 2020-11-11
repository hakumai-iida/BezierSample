bmp2hif /UC _source\system\debug_circle.bmp
move /Y _source\system\debug_circle*.hif IMAGE\.

bmp2hif /UC /0x06_80 _source\system\debug_ui.bmp
move /Y _source\system\debug_ui*.hif IMAGE\.

bmp2hif /UC _source\system\debug_font.bmp
move /Y _source\system\debug_font*.hif IMAGE\.

bmp2hif /UC _source\pal\pal_bezier_color.bmp
move /Y _source\pal\pal_bezier_color*.hif IMAGE\.

bmp2hif /UC _source\pal\pal_bezier_work_path.bmp
move /Y _source\pal\pal_bezier_work_path*.hif IMAGE\.

