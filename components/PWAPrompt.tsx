'use client'

import { useEffect, useState } from 'react'

export default function PWAPrompt() {
  const [deferredPrompt, setDeferredPrompt] = useState<any>(null)
  const [showPrompt, setShowPrompt] = useState(false)

  useEffect(() => {
    const handler = (e: Event) => {
      e.preventDefault()
      setDeferredPrompt(e)
      setShowPrompt(true)
    }

    window.addEventListener('beforeinstallprompt', handler)

    return () => {
      window.removeEventListener('beforeinstallprompt', handler)
    }
  }, [])

  const handleInstall = async () => {
    if (!deferredPrompt) return

    deferredPrompt.prompt()
    const result = await deferredPrompt.userChoice
    
    if (result.outcome === 'accepted') {
      setShowPrompt(false)
    }
    
    setDeferredPrompt(null)
  }

  if (!showPrompt) return null

  return (
    <div className="fixed bottom-4 left-4 right-4 bg-blue-600 text-white p-4 rounded-lg shadow-lg flex items-center justify-between z-50">
      <div>
        <h3 className="font-semibold">Instalar App</h3>
        <p className="text-sm text-blue-100">
          Instala la app para acceso rápido y notificaciones
        </p>
      </div>
      <div className="flex space-x-2">
        <button
          onClick={() => setShowPrompt(false)}
          className="px-3 py-1 text-sm bg-blue-500 rounded hover:bg-blue-400"
        >
          Ahora no
        </button>
        <button
          onClick={handleInstall}
          className="px-3 py-1 text-sm bg-white text-blue-600 rounded hover:bg-gray-100"
        >
          Instalar
        </button>
      </div>
    </div>
  )
}
