cmd_/home/pablo/Escritorio/ASO/P3/Module.symvers := sed 's/\.ko$$/\.o/' /home/pablo/Escritorio/ASO/P3/modules.order | scripts/mod/modpost -m -a  -o /home/pablo/Escritorio/ASO/P3/Module.symvers -e -i Module.symvers   -T -